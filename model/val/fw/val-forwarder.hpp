/**
 * jfp 2019
 * mieti - uminho
 */
#ifndef VAL_FW_FORWARDER_HPP
#define VAL_FW_FORWARDER_HPP


#include "NFD/daemon/fw/face-table.hpp"
#include "NFD/daemon/fw/forwarder.hpp"

#include "../../ndn-l3-protocol.hpp"

#include "../val-packet.hpp"
#include "../table/val-ifnt.hpp"
#include "../face/val-geoface-factory.hpp"

#include <list>

namespace ndn {
class Data;
class Interest;
}

namespace ns3 {
namespace ndn {
namespace val {


class ValForwarder
{

public:
    ValForwarder(L3Protocol& l3P);
    ~ValForwarder();
public:
    void
    reveiceInterest(const Interest& interest);

    void
    reveiceData(const Data& data, std::vector<const uint32_t> *nonceList, bool isProducer);

private:
    void
    onReceivedValPacket(const Face& face, const Block& valP);

    void
    processInterestFromNetwork(const Face& face, const Block& valH, const Interest& interest);

    void
    processDataFromNetwork(const Face& face, const Block& valH, const Data& data);

    void
    cleanupOnFaceRemoval(const Face& face);

    void
    addToNetworkFaceList(const Face& face);

    void
    removeFromNetworkFaceList(const Face& face);

    Face*
    getNetworkFace(nfd::FaceId faceId);

private:
    L3Protocol *m_l3P; // add faces with this
    nfd::FaceTable *m_faceTable;
    ifnt::Ifnt m_ifnt;
    std::shared_ptr<nfd::face::Face> m_geoface;
    face::GeofaceFactory m_geofaceFactory;
    std::list<nfd::FaceId> m_networkFaces;
    // counters
    uint32_t m_invalidIN;
};

                
} // namespace val            
} // namespace ndn        
} // namespace ns3

#endif