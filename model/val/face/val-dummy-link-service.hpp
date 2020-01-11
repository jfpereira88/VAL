/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_FACE_DUMMY_LINK_SERVICE_HPP
#define VAL_FACE_DUMMY_LINK_SERVICE_HPP

#include "NFD/daemon/face/link-service.hpp"
#include "../fw/val-forwarder.hpp"

namespace ns3 {
namespace ndn {
namespace val {
class ValForwarder;
namespace face {

using ::nfd::face::LinkService;
using ::nfd::face::Transport;
    
class ValDummyLinkService : public LinkService
{
public:
    ValDummyLinkService(ValForwarder& valFwd);
    
    ~ValDummyLinkService() override;

private:
    void
    doSendInterest(const Interest& interest) override;
    
    void
    doSendData(const Data& data) override;

    void
    doSendNack(const lp::Nack& nack) override;

    void
    doReceivePacket(Transport::Packet&& packet) override;

    void
    doSendDataToVal(const Data& data, std::vector<uint32_t> *nonceList, bool isProcucer) override;

    void
    doSendInterestToVal(const Interest& interest) override;


private:
    ValForwarder* m_valFwd;
};



} // namespace face
} // namespace val
} // namespace ndn
} // namespace ns3

#endif