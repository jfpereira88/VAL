/**
 * jfp 2019
 * mieti - uminho
 */
#include "val-dummy-link-service.hpp"
#include "ns3/log.h"

namespace ns3 {
namespace ndn {
namespace val {
namespace face {
    
ValDummyLinkService::ValDummyLinkService(ValForwarder& valFwd)
    : m_valFwd(&valFwd)
{
}

ValDummyLinkService::~ValDummyLinkService()
{
}

void
ValDummyLinkService::doSendDataToVal(const Data& data, std::vector<const uint32_t> *nonceList)
{
    // code here
    return;
}

void
ValDummyLinkService::doSendInterestToVal(const Interest& interest)
{
    return;
}


// override pure virtual methods
void
ValDummyLinkService::doSendInterest(const Interest& interest)
{
    NS_LOG_ERROR("ValDummyLinkService::doSendInterest -> This should never happend!!!");
    BOOST_ASSERT(false);
}

void
ValDummyLinkService::doSendData(const Data& data)
{
    NS_LOG_ERROR("ValDummyLinkService::doSendData -> This should never happend!!!");
    BOOST_ASSERT(false);
}

void
ValDummyLinkService::doSendNack(const lp::Nack& nack)
{
    NS_LOG_ERROR("ValDummyLinkService::doSendNack -> This should never happend!!!");
    BOOST_ASSERT(false);
}

void
ValDummyLinkService::doReceivePacket(Transport::Packet&& packet)
{
    NS_LOG_ERROR("ValDummyLinkService::doReceivePacket -> This should never happend!!!");
    BOOST_ASSERT(false);
}

} // namespace face
} // namespace val
} // namespace ndn
} // namespace ns3
