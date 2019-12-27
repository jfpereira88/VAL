/**
 * jfp 2019
 * mieti - uminho
 */

#include "val-dfnt-entry.hpp"

namespace ns3 {
namespace ndn {
namespace val {
namespace dfnt {


Entry::Entry(const ::ndn::Data& data, ::nfd::FaceId faceId)
    : m_sigInfo(data.getSignature().getSignatureInfo())
    , m_sa(0)
    , m_faceId(faceId)
{
}

Entry::~Entry()
{
}

const ::ndn::SignatureInfo
Entry::getSignatureInfo() const 
{
    return m_sigInfo;
}

const uint32_t
Entry::getSA() const 
{
    return m_sa;
}

const ::nfd::FaceId
Entry::getFaceId() const 
{
    return m_faceId;
}

 bool
operator==(const Entry& l_entry, const Entry& r_entry)
{
    return (l_entry.getSignatureInfo() == r_entry.getSignatureInfo() && 
            l_entry.getSA() == r_entry.getSA() && 
            l_entry.getFaceId() == r_entry.getFaceId());
}

} // namespace dfnt
} // namespace val    
} // namespace ndn
} // namespace ns3
