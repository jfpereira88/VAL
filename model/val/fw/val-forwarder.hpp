/**
 * jfp 2019
 * mieti - uminho
 */
#ifndef VAL_FW_FORWARDER_HPP
#define VAL_FW_FORWARDER_HPP


#include "NFD/daemon/fw/face-table.hpp"
#include "NFD/daemon/fw/forwarder.hpp"


#include "../val-header.hpp"
#include "../table/val-ifnt.hpp"
#include "../table/val-dfnt.hpp"
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


class ValForwarder
{

public:
    ValForwarder(ndn::L3Protocol& l3P);
    ~ValForwarder();
public:
    void
    reveiceInterest(const nfd::Face *inGeoface, const Interest& interest);

    void
    reveiceData(const nfd::Face *inGeoface, const Data& data, std::vector<const uint32_t> *nonceList, bool isProducer);

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

    Face*
    getOtherNetworkFace(nfd::FaceId faceId);

private:
    ndn::L3Protocol *m_l3P; // add faces with this
    nfd::FaceTable *m_faceTable;
    ifnt::Ifnt m_ifnt;
    dfnt::Dfnt m_dfnt;
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