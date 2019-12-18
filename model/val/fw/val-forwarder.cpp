/**
 * jfp 2019
 * mieti - uminho
 */

#include "val-forwarder.hpp"



namespace ns3 {
namespace ndn {
namespace val {

namespace tlv {
    using namespace ::ndn::tlv;
} // namespace tlv

using ::ndn::Interest;
using ::ndn::Data;

NFD_LOG_INIT(ValForwarder);

ValForwarder::ValForwarder(ndn::L3Protocol& l3P)
    : m_l3P(&l3P)
    , m_invalidIN(0)
{
    m_faceTable = &(m_l3P->getForwarder()->getFaceTable());
    m_faceTable->afterValFaceAdd.connect([this] (Face& face) {
    face.afterReceiveValPkt.connect(
      [this, &face] (const ndn::Block& valP) {
        onReceivedValPacket(face, valP);
      });
    });

    m_faceTable->beforeRemove.connect([this] (Face& face) {
        cleanupOnFaceRemoval(face);
    });
}

ValForwarder::~ValForwarder()
{
}

void
ValForwarder::onReceivedValPacket(const Face& face, const ndn::Block& valP)
{
  //@TODO valP must be parsed to extrect ValPacket Headers
  try {
    switch (valP.type()) {
      case tlv::Interest:
      { // braces to avoid "transfer of control bypasses" problem
        // forwarding expects Interest to be created with make_shared
        auto interest = make_shared<Interest>(valP);
        processReceivedInterest(face, valP, *interest);
        break;
      }
      case tlv::Data:
      {
        // forwarding expects Interest to be created with make_shared
        auto data = make_shared<Data>(valP);
        processReceivedData(face, valP, *data);
        break;
      }
      default:
        m_invalidIN++;
        NFD_LOG_DEBUG("unrecognized network-layer packet TLV-TYPE " << valP.type() << ": DROP");
        return;
    }
  }
  catch (const tlv::Error& e) {
    m_invalidIN++;
    NFD_LOG_DEBUG("packet parse error (" << e.what() << "): DROP");
  }
}

void
ValForwarder::processReceivedInterest(const Face& face, const Block& valH, const Interest& interest)
{
  // add ifnt entry
  ifnt::Entry entry(interest, face);
  m_ifnt.addEntry(entry);
  
}

void
ValForwarder::processReceivedData(const Face& face, const Block& valH, const Data& data)
{

}

void
ValForwarder::cleanupOnFaceRemoval(const Face& face)
{
  //do something
}

} // namespace val            
} // namespace ndn        
} // namespace ns3