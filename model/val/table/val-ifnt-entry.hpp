/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_TABLE_IFNT_ENTRY_HPP
#define VAL_TABLE_IFNT_ENTRY_HPP

#include <ndn-cxx/interest.hpp>
#include <string>

#include "../val-header.hpp"


namespace ns3 {
namespace ndn {
namespace val {
namespace ifnt {

class Entry
{

public:
    Entry(const ValHeader& valHeader, const ::ndn::Interest& interest, uint64_t faceId);
    ~Entry();

    const std::string
    getDA() const;

    const std::string
    getSA() const;

    const uint32_t
    getNonce() const;

    const std::string
    getPhPos() const;

    const uint8_t
    getHopC() const;

    const std::string
    getRN() const;

    const uint64_t
    getFaceId() const;

private:
    std::string m_da;
    std::string m_sa;        // source area
    uint32_t m_nonce;        // nonce
    std::string m_phPos;     // previous hop position
    uint8_t m_hopC;          // hop count
    std::string m_rn;        // root name
    uint64_t m_faceId;
};

bool
operator==(const Entry& l_entry, const Entry& r_entry);

} // namespace ifnt
} // namespace val    
} // namespace ndn
} // namespace ns3



#endif