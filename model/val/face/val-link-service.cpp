/**
 * jfp 2019
 * mieti - uminho
 */

#include "val-link-service.hpp"

#include <ndn-cxx/lp/tags.hpp>

#include <cmath>

namespace ns3 {
namespace ndn {
namespace val {
namespace face {


NFD_LOG_INIT(ValLinkService);

constexpr uint32_t DEFAULT_CONGESTION_THRESHOLD_DIVISOR = 2;

ValLinkService::ValLinkService(const ValLinkService::Options& options)
  : m_options(options)
  , m_fragmenter(m_options.fragmenterOptions, this)
  , m_reassembler(m_options.reassemblerOptions, this)
  , m_lastSeqNo(-2)
  , m_nextMarkTime(time::steady_clock::TimePoint::max())
  , m_lastMarkTime(time::steady_clock::TimePoint::min())
  , m_nMarkedSinceInMarkingState(0)
{
  m_reassembler.beforeTimeout.connect([this] (auto...) { ++this->nReassemblyTimeouts; });
  nReassembling.observe(&m_reassembler);
}

void
ValLinkService::setOptions(const ValLinkService::Options& options)
{
  m_options = options;
  m_fragmenter.setOptions(m_options.fragmenterOptions);
  m_reassembler.setOptions(m_options.reassemblerOptions);
}


void
ValLinkService::sendLpPacket(lp::Packet&& pkt)
{
  const ssize_t mtu = this->getTransport()->getMtu();

  if (m_options.allowCongestionMarking) {
    checkCongestionLevel(pkt);
  }

  Transport::Packet tp(pkt.wireEncode());
  if (mtu != MTU_UNLIMITED && tp.packet.size() > static_cast<size_t>(mtu)) {
    ++this->nOutOverMtu;
    NFD_LOG_FACE_WARN("attempted to send packet over MTU limit");
    return;
  }
  this->sendPacket(std::move(tp));
}

void
ValLinkService::encodeLpFields(const ndn::PacketBase& valPkt, lp::Packet& lpPacket)
{
  if (m_options.allowLocalFields) {
    shared_ptr<lp::IncomingFaceIdTag> incomingFaceIdTag = valPkt.getTag<lp::IncomingFaceIdTag>();
    if (incomingFaceIdTag != nullptr) {
      lpPacket.add<lp::IncomingFaceIdField>(*incomingFaceIdTag);
    }
  }

  shared_ptr<lp::CongestionMarkTag> congestionMarkTag = valPkt.getTag<lp::CongestionMarkTag>();
  if (congestionMarkTag != nullptr) {
    lpPacket.add<lp::CongestionMarkField>(*congestionMarkTag);
  }

  if (m_options.allowSelfLearning) {
    shared_ptr<lp::NonDiscoveryTag> nonDiscoveryTag = valPkt.getTag<lp::NonDiscoveryTag>();
    if (nonDiscoveryTag != nullptr) {
      lpPacket.add<lp::NonDiscoveryField>(*nonDiscoveryTag);
    }

    shared_ptr<lp::PrefixAnnouncementTag> prefixAnnouncementTag = valPkt.getTag<lp::PrefixAnnouncementTag>();
    if (prefixAnnouncementTag != nullptr) {
      lpPacket.add<lp::PrefixAnnouncementField>(*prefixAnnouncementTag);
    }
  }

  shared_ptr<lp::HopCountTag> hopCountTag = valPkt.getTag<lp::HopCountTag>();
  if (hopCountTag != nullptr) {
    lpPacket.add<lp::HopCountTagField>(*hopCountTag);
  }
  else {
    lpPacket.add<lp::HopCountTagField>(0);
  }
}

void
ValLinkService::sendValPacket(lp::Packet&& pkt)
{
  std::vector<lp::Packet> frags;
  ssize_t mtu = this->getTransport()->getMtu();

  if (m_options.allowCongestionMarking && mtu != MTU_UNLIMITED) {
    mtu -= CONGESTION_MARK_SIZE;
  }

  BOOST_ASSERT(mtu == MTU_UNLIMITED || mtu > 0);

  if (m_options.allowFragmentation && mtu != MTU_UNLIMITED) {
    bool isOk = false;
    std::tie(isOk, frags) = m_fragmenter.fragmentPacket(pkt, mtu);
    if (!isOk) {
      // fragmentation failed (warning is logged by LpFragmenter)
      ++this->nFragmentationErrors;
      return;
    }
  }
  else {
      frags.push_back(std::move(pkt));
  }

  if (frags.size() == 1) {
    // even if indexed fragmentation is enabled, the fragmenter should not
    // fragment the packet if it can fit in MTU
    BOOST_ASSERT(!frags.front().has<lp::FragIndexField>());
    BOOST_ASSERT(!frags.front().has<lp::FragCountField>());
  }

  // Only assign sequences to fragments if packet contains more than 1 fragment
  if (frags.size() > 1) {
    // Assign sequences to all fragments
    this->assignSequences(frags);
  }

  //if (m_options.reliabilityOptions.isEnabled && frags.front().has<lp::FragmentField>()) {
  //  m_reliability.handleOutgoing(frags, std::move(pkt), isInterest);
  //}

  for (lp::Packet& frag : frags) {
    this->sendLpPacket(std::move(frag));
  }
}

void
ValLinkService::assignSequence(lp::Packet& pkt)
{
  pkt.set<lp::SequenceField>(++m_lastSeqNo);
}

void
ValLinkService::assignSequences(std::vector<lp::Packet>& pkts)
{
  std::for_each(pkts.begin(), pkts.end(), [this] (auto& pkt) { this->assignSequence(pkt); });
}

void
ValLinkService::checkCongestionLevel(lp::Packet& pkt)
{
  ssize_t sendQueueLength = getTransport()->getSendQueueLength();
  // This operation requires that the transport supports retrieving current send queue length
  if (sendQueueLength < 0) {
    return;
  }

  // To avoid overflowing the queue, set the congestion threshold to at least half of the send
  // queue capacity.
  size_t congestionThreshold = m_options.defaultCongestionThreshold;
  if (getTransport()->getSendQueueCapacity() >= 0) {
    congestionThreshold = std::min(congestionThreshold,
                                   static_cast<size_t>(getTransport()->getSendQueueCapacity()) /
                                                       DEFAULT_CONGESTION_THRESHOLD_DIVISOR);
  }

  if (sendQueueLength > 0) {
    NFD_LOG_FACE_TRACE("txqlen=" << sendQueueLength << " threshold=" << congestionThreshold <<
                       " capacity=" << getTransport()->getSendQueueCapacity());
  }

  if (static_cast<size_t>(sendQueueLength) > congestionThreshold) { // Send queue is congested
    const auto now = time::steady_clock::now();
    if (now >= m_nextMarkTime || now >= m_lastMarkTime + m_options.baseCongestionMarkingInterval) {
      // Mark at most one initial packet per baseCongestionMarkingInterval
      if (m_nMarkedSinceInMarkingState == 0) {
        m_nextMarkTime = now;
      }

      // Time to mark packet
      pkt.set<lp::CongestionMarkField>(1);
      ++nCongestionMarked;
      NFD_LOG_FACE_DEBUG("LpPacket was marked as congested");

      ++m_nMarkedSinceInMarkingState;
      // Decrease the marking interval by the inverse of the square root of the number of packets
      // marked in this incident of congestion
      m_nextMarkTime += time::nanoseconds(static_cast<time::nanoseconds::rep>(
                                            m_options.baseCongestionMarkingInterval.count() /
                                            std::sqrt(m_nMarkedSinceInMarkingState)));
      m_lastMarkTime = now;
    }
  }
  else if (m_nextMarkTime != time::steady_clock::TimePoint::max()) {
    // Congestion incident has ended, so reset
    NFD_LOG_FACE_DEBUG("Send queue length dropped below congestion threshold");
    m_nextMarkTime = time::steady_clock::TimePoint::max();
    m_nMarkedSinceInMarkingState = 0;
  }
}

void
ValLinkService::doReceivePacket(Transport::Packet&& packet)
{
  try {
    lp::Packet pkt(packet.packet);

    if (!pkt.has<lp::FragmentField>()) {
      NFD_LOG_FACE_TRACE("received IDLE packet: DROP");
      return;
    }

    if ((pkt.has<lp::FragIndexField>() || pkt.has<lp::FragCountField>()) &&
        !m_options.allowReassembly) {
      NFD_LOG_FACE_WARN("received fragment, but reassembly disabled: DROP");
      return;
    }

    bool isReassembled = false;
    Block valPkt;
    lp::Packet firstPkt;
    std::tie(isReassembled, valPkt, firstPkt) = m_reassembler.receiveFragment(packet.remoteEndpoint,
                                                                              pkt);
    if (isReassembled) {
        // send to val-forwarder here
        ++this->nInValPkt;
        this->receiveValPkt(std::move(valPkt));
       
    }
  }
  catch (const tlv::Error& e) {
    ++this->nInLpInvalid;
    NFD_LOG_FACE_WARN("packet parse error (" << e.what() << "): DROP");
  }
}





} // namespace face
} // namespace val
}
}