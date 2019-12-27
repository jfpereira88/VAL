/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_TABLE_DFNT_ENTRY_HPP
#define VAL_TABLE_DFNT_ENTRY_HPP

#include <ndn-cxx/data.hpp>
#include <ndn-cxx/signature-info.hpp>
#include "ns3/ndnSIM/NFD/daemon/face/face.hpp"


namespace ns3 {
namespace ndn {
namespace val {
namespace dfnt {

class Entry
{

public:
    Entry(const ::ndn::Data& data, ::nfd::FaceId faceId);
    ~Entry();

    const ::ndn::SignatureInfo
    getSignatureInfo() const;

    const uint32_t
    getSA() const;

    const ::nfd::FaceId
    getFaceId() const;

private:
    ::ndn::SignatureInfo m_sigInfo;
    uint32_t m_sa;
    ::nfd::FaceId m_faceId;
};

bool
operator==(const Entry& l_entry, const Entry& r_entry);

} // namespace dfnt
} // namespace val    
} // namespace ndn
} // namespace ns3



#endif