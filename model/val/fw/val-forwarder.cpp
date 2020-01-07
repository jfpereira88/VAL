/**
 * jfp 2019
 * mieti - uminho
 */

#include "val-forwarder.hpp"
#include "ns3/ndnSIM/model/ndn-l3-protocol.hpp"
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
    NS_LOG_DEBUG("Creating VAL"); 
    m_faceTable = &(m_l3P->getForwarder()->getFaceTable());
    m_faceTable->afterValFaceAdd.connect([this] (Face& face) {
      NS_LOG_DEBUG("Adding ValNetFace? "<< std::boolalpha << face.isValNetFace());
      addToNetworkFaceList(face);
      face.afterReceiveValPkt.connect(
        [this, &face] (const ValPacket& valP) {
          onReceivedValPacket(face, valP);
        });
    });

    m_faceTable->beforeValFaceRemove.connect([this] (Face& face) {
        cleanupOnFaceRemoval(face);
    });
    // TODO: it is possible to know if a geoface have been removed by NFD,
    // beforeFaceRemove check this
    m_geoface = m_geofaceFactory.makeGeoface(); //just for now
    m_l3P->addFace(m_geoface); //temp
    nfd::fib::Entry* entry = m_l3P->getForwarder()->getFib().insert("/").first;
    entry->addOrUpdateNextHop(*m_geoface, 0, 1);
}

ValForwarder::~ValForwarder()
{
}

void
ValForwarder::onReceivedValPacket(const Face& face, const ValPacket& valP)
{
  NS_LOG_DEBUG(__func__);
  //@TODO valP must be parsed to extrect ValPacket Headers
  try {
    switch (valP.isSet()) {
      case ValPacket::INTEREST_SET:
      { // braces to avoid "transfer of control bypasses" problem
        // forwarding expects Interest to be created with make_shared
        auto interest = make_shared<Interest>(valP.getInterest());
        processInterestFromNetwork(face, valP.getValHeader(), *interest);
        break;
      }
      case ValPacket::DATA_SET:
      {
        // forwarding expects Interest to be created with make_shared
        auto data = make_shared<Data>(valP.getData());
        processDataFromNetwork(face, valP.getValHeader(), *data);
        break;
      }
      default:
        m_invalidIN++;
        NS_LOG_DEBUG("unrecognized network-layer packet TLV-TYPE " << valP.isSet() << ": DROP");
        return;
    }
  }
  catch (const tlv::Error& e) {
    m_invalidIN++;
    NS_LOG_DEBUG("packet parse error (" << e.what() << "): DROP");
  }
}

void
ValForwarder::processInterestFromNetwork(const Face& face, const ValHeader& valH, const Interest& interest)
{
  NS_LOG_DEBUG(__func__);
  // add ifnt entry
  ifnt::Entry entry(interest, face.getId());
  m_ifnt.addEntry(entry);
  m_geoface->sendInterestToForwarder(std::move(interest)); // temp
}

void
ValForwarder::processDataFromNetwork(const Face& face, const ValHeader& valH, const Data& data)
{
  NS_LOG_DEBUG(__func__);
  dfnt::Entry entry(valH, data, face.getId());
  m_dfnt.addEntry(std::move(entry));
  m_geoface->sendDataToForwarder(std::move(data));
}

void
ValForwarder::reveiceInterest(const nfd::Face *inGeoface, const Interest& interest)
{
  NS_LOG_DEBUG(__func__);
  auto pair = m_ifnt.findMatchByNonce(interest.getNonce());
  ValHeader valH;
  valH.setSA(interest.getName().toUri());
  valH.setDA(std::to_string(interest.getNonce()));
  ValPacket valPkt(valH);
  valPkt.setInterest(std::make_shared<Interest>(interest));
  size_t size = m_networkFaces.size();
  NS_LOG_DEBUG("size of NetworkFaces Table: "<< size);
  if (pair.second) {
    NS_LOG_DEBUG("Interest from network");
    if(size > 1){
      // get a network face diferent from the incoming network face
      Face* face = getOtherNetworkFace(pair.first->getFaceId());
      if(face != nullptr && face->isValNetFace()){
        NS_LOG_DEBUG("Send Valpkt to ValNetFace, "<< face->getId() << "incomming Face " << pair.first->getFaceId());
        face->sendValPacket(std::move(valPkt)); 
      }
    } else {
      Face* face = getNetworkFace(*m_networkFaces.begin());
      if(face != nullptr && face->isValNetFace()){
        face->sendValPacket(std::move(valPkt)); 
      }
    }
  } else {
    NS_LOG_DEBUG("Interest generated locally");
    Face* face = getNetworkFace(*m_networkFaces.begin());
    if(face == nullptr)
      NS_LOG_DEBUG("NetValFace is null");
    NS_LOG_DEBUG("isValNetFace? "<< std::boolalpha << face->isValNetFace());
    if(face != nullptr && face->isValNetFace()){
      NS_LOG_DEBUG("Send Valpkt to ValNetFace, "<< face->getId());
      face->sendValPacket(std::move(valPkt)); 
    }
  }
  
  /*if(pair.second) {
    Face* face = getNetworkFace(pair.first->getFaceId());
    if(face != nullptr && face->isValNetFace()){
      face->sendValPacket(std::move(valPkt)); 
    }
  } else { // Interest was generated locally
    // first network face to be added
    Face* face = getNetworkFace(*m_networkFaces.begin());
    if(face != nullptr && face->isValNetFace()){
      face->sendValPacket(std::move(valPkt)); 
    }
  }*/
}

void
ValForwarder::reveiceData(const nfd::Face *inGeoface, const Data& data, std::vector<const uint32_t> *nonceList, bool isProducer)
{
  NS_LOG_DEBUG(__func__);
  ValHeader valHeader;
  valHeader.setRN(data.getName().toUri());
  valHeader.setPhPos(std::to_string(data.getSignature().getType()));
  ValPacket valPkt(valHeader);
  valPkt.setData(std::make_shared<Data>(data));
  auto entry = m_dfnt.findMatch(data.getSignature(), 0);
  size_t size = m_networkFaces.size();
  if(entry.first) {
    NS_LOG_DEBUG("Data from network");
    if(size > 1){
      // get a network face diferent from the incoming network face
      Face* face = getOtherNetworkFace(entry.second.getFaceId());
      if(face != nullptr && face->isValNetFace()){
        face->sendValPacket(std::move(valPkt)); 
      }
    } else {
      Face* face = getNetworkFace(*m_networkFaces.begin());
      if(face != nullptr && face->isValNetFace()){
        face->sendValPacket(std::move(valPkt)); 
      }
    }
  } else {
    NS_LOG_DEBUG("Data generated locally");
    Face* face = getNetworkFace(*m_networkFaces.begin());
    if(face != nullptr && face->isValNetFace()){
      face->sendValPacket(std::move(valPkt)); 
    }
  }
  /*
  Face* face = getNetworkFace(*m_networkFaces.begin());
  if(face != nullptr && face->isValNetFace()){
    face->sendValPacket(std::move(valPkt)); 
  }*/
}

void
ValForwarder::cleanupOnFaceRemoval(const Face& face)
{
  removeFromNetworkFaceList(face);
}

Face*
ValForwarder::getNetworkFace(nfd::FaceId faceId)
{
  NS_LOG_DEBUG(__func__ << "FaceId: " << faceId);
  auto it = m_networkFaces.begin();
  while (it != m_networkFaces.end()) {
    if (*it == faceId) {
      auto it_faces = m_faceTable->begin();
      while (it_faces != m_faceTable->end()) {
        if (it_faces->getId() == faceId) {
          return &*it_faces;
        }
        it_faces++;
      }
    }
    it++;
  }
  return nullptr;
}

Face*
ValForwarder::getOtherNetworkFace(nfd::FaceId faceId)
{
  NS_LOG_DEBUG(__func__);
  auto it = m_networkFaces.begin();
  while (it != m_networkFaces.end()) {
    if (*it != faceId) {
      auto it_faces = m_faceTable->begin();
      while (it_faces != m_faceTable->end()) {
        if (it_faces->getId() == *it) {
          return &*it_faces;
        }
        it_faces++;
      }
    }
    it++;
  }
  return nullptr;
}

void
ValForwarder::addToNetworkFaceList(const Face& face)
{
  NS_LOG_DEBUG(__func__);
  m_networkFaces.push_back(face.getId());
}

void
ValForwarder::removeFromNetworkFaceList(const Face& face)
{
  NS_LOG_DEBUG(__func__);
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