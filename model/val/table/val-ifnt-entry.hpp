/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_TABLE_IFNT_ENTRY_HPP
#define VAL_TABLE_IFNT_ENTRY_HPP

#include <memory>
#include <ndn-cxx/interest.hpp>

namespace nfd {
namespace face {
    class Face;
}
}

using ::ndn::Interest;

namespace ns3 {
namespace ndn {
namespace val {
namespace ifnt {

class Entry
{
public:
    explicit
    Entry(const Interest& interest, const nfd::face::Face& face);

    Entry(const Entry& entry);

    ~Entry();
    
    const uint32_t
    getNonce() const
    {
        return m_nonce;
    }

    const uint32_t
    getDA() const
    {
        return m_da;
    }

    const nfd::face::Face&
    getFace() const
    {
        return m_face;
    }
    
private:
    uint32_t m_nonce;
    uint32_t m_da;
    // unmutable pointer to const variable
    const nfd::face::Face& m_face;
};

bool
operator==(const Entry& l_entry, const Entry& r_entry);

bool
operator==(std::shared_ptr<const Entry> l_entry, const Entry& r_entry);

inline bool
operator!=(const Entry& l_entry, const Entry& r_entry)
{
  return !(l_entry == r_entry);
}

} // namespace ifnt
} // namespace val
} // namespace ndn
} // namespace ns3

#endif