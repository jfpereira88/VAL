/**
 * jfp 2019
 * mieti - uminho
 */

#include "val-forwarder.hpp"
#include "ns3/log.h"

NS_LOG_COMPONENT_DEFINE("ndn.val.ValForwarder");

namespace ns3 {
namespace ndn {
namespace val {

namespace tlv {
    using namespace ::ndn::tlv;
} // namespace tlv

using ::ndn::Interest;
using ::ndn::Data;


ValForwarder::ValForwarder(L3Protocol& l3P)
    : m_l3P(&l3P)
    , m_geofaceFactory(*this)
    , m_invalidIN(0)
{
    m_faceTable = &(m_l3P->getForwarder()->getFaceTable());
    m_faceTable->afterValFaceAdd.connect([this] (Face& face) {
      addToNetworkFaceList(face);
      face.afterReceiveValPkt.connect(
        [this, &face] (const ndn::Block& valP) {
          onReceivedValPacket(face, valP);
        });
    });

    m_faceTable->beforeValFaceRemove.connect([this] (Face& face) {
        cleanupOnFaceRemoval(face);
    });
    // TODO: it is possible to know if a geoface have been removed,
    // beforeFaceRemove check this
    m_geoface = m_geofaceFactory.makeGeoface(); //just for now
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
        processInterestFromNetwork(face, valP, *interest);
        break;
      }
      case tlv::Data:
      {
        // forwarding expects Interest to be created with make_shared
        auto data = make_shared<Data>(valP);
        processDataFromNetwork(face, valP, *data);
        break;
      }
      default:
        m_invalidIN++;
        NS_LOG_DEBUG("unrecognized network-layer packet TLV-TYPE " << valP.type() << ": DROP");
        return;
    }
  }
  catch (const tlv::Error& e) {
    m_invalidIN++;
    NS_LOG_DEBUG("packet parse error (" << e.what() << "): DROP");
  }
}

void
ValForwarder::processInterestFromNetwork(const Face& face, const Block& valH, const Interest& interest)
{
  // add ifnt entry
  ifnt::Entry entry(interest, face.getId());
  m_ifnt.addEntry(entry);
  m_geoface->sendInterestToForwarder(std::move(interest)); // temp
}

void
ValForwarder::processDataFromNetwork(const Face& face, const Block& valH, const Data& data)
{
  m_geoface->sendDataToForwarder(std::move(data));
}

void
ValForwarder::reveiceInterest(const Interest& interest)
{
  auto pair = m_ifnt.findMatchByNonce(interest.getNonce());
  ::ndn::Block valPkt(interest.wireEncode());
  if(pair.second) {
    Face* face = getNetworkFace(pair.first->getFaceId());
    if(face != nullptr && face->isValNetFace()){
      face->sendValPacket(std::move(valPkt)); // no Val packet yet
    }
  } else { // Interest was generated locally
    // first network face to be added
    Face* face = getNetworkFace(*m_networkFaces.begin());
    if(face != nullptr && face->isValNetFace()){
      face->sendValPacket(std::move(valPkt)); // no Val packet yet
    }
  }
}

void
ValForwarder::reveiceData(const Data& data, std::vector<const uint32_t> *nonceList, bool isProducer)
{
  ::ndn::Block valPkt(data.wireEncode());
  Face* face = getNetworkFace(*m_networkFaces.begin());
    if(face != nullptr && face->isValNetFace()){
      face->sendValPacket(std::move(valPkt)); // no Val packet yet
    }
}

void
ValForwarder::cleanupOnFaceRemoval(const Face& face)
{
  removeFromNetworkFaceList(face);
}

Face*
ValForwarder::getNetworkFace(nfd::FaceId faceId)
{
  auto it = m_networkFaces.begin();
  while (it != m_networkFaces.end()) {
    if (*it == faceId) {
      auto it_faces = m_faceTable->begin();
      while (it_faces != m_faceTable->end()) {
        if (it_faces->getId() == faceId) {
          return &*it_faces;
        }
      }
    }
  }
  return nullptr;
}

void
ValForwarder::addToNetworkFaceList(const Face& face)
{
  m_networkFaces.push_back(face.getId());
}

void
ValForwarder::removeFromNetworkFaceList(const Face& face)
{
  auto it = m_networkFaces.begin();
  while (it != m_networkFaces.end()){
    if(*it == face.getId()) {
      m_networkFaces.erase(it);
      break;
    }
    it++;
  }
}

} // namespace val            
} // namespace ndn        
} // namespace ns3