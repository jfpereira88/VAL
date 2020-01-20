/**
 * jfp 2019
 * mieti - uminho
 */
#ifndef VAL_FW_FORWARDER_HPP
#define VAL_FW_FORWARDER_HPP


#include "NFD/daemon/fw/face-table.hpp"
#include "NFD/daemon/fw/forwarder.hpp"
#include "NFD/core/scheduler.hpp"

#include "../val-api.hpp"
#include "../val-packet.hpp"
#include "../table/val-ifnt.hpp"
#include "../table/val-dfnt.hpp"
#include "../table/val-pft.hpp"
#include "../table/val-f2a.hpp"
#include "../face/val-geoface-factory.hpp"

#include <list>

namespace ndn {
class Data;
class Interest;
}

namespace ns3 {
namespace ndn {
    class L3Protocol;
namespace val {
    class ValStrategy;

using namespace ::ndn::literals::time_literals;
//namespace time = ::ndn::time;

class ValForwarder
{
friend class ValStrategy;

public:
    ValForwarder(ndn::L3Protocol& l3P);
    ~ValForwarder();

    void
    reveiceInterest(const nfd::Face *inGeoface, const Interest& interest);

    void
    reveiceData(const nfd::Face *inGeoface, const Data& data, std::vector<uint32_t> *nonceList, bool isProducer);

    void
    registerOutgoingValPacket(const nfd::FaceId outFaceId, ValPacket& valPkt, time::nanoseconds duration);

private:
    void
    onReceivedValPacket(const Face& face, const ValPacket& valP);

    void
    processInterestFromNetwork(const Face& face, const ValHeader& valH, const Interest& interest);

    void
    processDataFromNetwork(const Face& face, const ValHeader& valH, const Data& data);

    void
    cleanupOnFaceRemoval(const Face& face);

    void
    addToNetworkFaceList(const Face& face);

    void
    removeFromNetworkFaceList(const Face& face);

    Face*
    getNetworkFace(nfd::FaceId faceId);

    Face*
    getOtherNetworkFace(nfd::FaceId faceId);

    //received packet path
    bool
    preProcessingValPacket(const Face& face, const ValPacket& valP);

    
    // received Interest from network path
    bool
    isEscapedPacket(const ValHeader& valH);

    void
    forwardingTimerCallback(const std::shared_ptr<pft::Entry>& pftEntry, const nfd::FaceId outFaceId);

    void
    impAckTimerCallback(const std::shared_ptr<pft::Entry>& pftEntry, const nfd::FaceId outFaceId);

    /** \brief Make geofaces from ValHeader
     * \return first FaceId_SA, second FaceID_DA
     */
    std::pair<uint32_t, uint32_t>
    makeGeoFaceFromValHeader(const ValHeader& valH);

private:
    ndn::L3Protocol *m_l3P; // add faces via this pointer
    nfd::FaceTable *m_faceTable;
    ifnt::Ifnt m_ifnt;
    dfnt::Dfnt m_dfnt;
    pft::PFT m_pft;
    f2a::F2A m_f2a;
    ValStrategy *m_strategy;
    face::GeofaceFactory m_geofaceFactory;
    std::list<nfd::FaceId> m_networkFaces;
    // counters
    uint32_t m_invalidIN;
};

                
} // namespace val            
} // namespace ndn        
} // namespace ns3

#endif