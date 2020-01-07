/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_TABLE_DFNT_ENTRY_HPP
#define VAL_TABLE_DFNT_ENTRY_HPP

#include <ndn-cxx/data.hpp>
#include <string>

#include "../val-header.hpp"


namespace ns3 {
namespace ndn {
namespace val {
namespace dfnt {

class Entry
{

public:
    Entry(const ValHeader& valHeader, const ::ndn::Data& data, uint64_t faceId);
    ~Entry();

    const std::string
    getSA() const;

    const ::ndn::Signature
    getSignature() const;

    const std::string
    getPhPos() const;

    const uint8_t
    getHopC() const;

    const std::string
    getRN() const;

    const uint64_t
    getFaceId() const;

private:
    std::string m_sa;        // source area
    ::ndn::Signature m_sig;  // signature
    std::string m_phPos;     // previous hop position
    uint8_t m_hopC;          // hop count
    std::string m_rn;        // root name
    uint64_t m_faceId;
};

bool
operator==(const Entry& l_entry, const Entry& r_entry);

} // namespace dfnt
} // namespace val    
} // namespace ndn
} // namespace ns3



#endif