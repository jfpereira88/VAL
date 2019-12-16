/**
 * jfp 2019
 * mieti - uminho
 */
#include "../../ndn-l3-protocol.hpp"
#include "NFD/daemon/fw/face-table.hpp"
#include "NFD/daemon/fw/forwarder.hpp"
#include "../val-packet.hpp"

namespace ns3 {
namespace ndn {
namespace val {

class ValForwarder
{

public:
    ValForwarder(ndn::L3Protocol& l3P);
    ~ValForwarder();
private:
    void
    onReceivedValPacket(const Face& face, const Block& valP);

    void
    processReceivedInterest(const Face& face, const Block& valH, const Interest& interest);

    void
    processReceivedData(const Face& face, const Block& valH, const Data& data);

    void
    cleanupOnFaceRemoval(const Face& face);

private:
    ndn::L3Protocol *m_l3P;
    nfd::FaceTable *m_faceTable;
    uint32_t m_invalidIN;
};

                
} // namespace val            
} // namespace ndn        
} // namespace ns3
