/**
 * jfp 2019
 * mieti - uminho
 */

#include "val-ifnt-entry.hpp"

namespace ns3 {
namespace ndn {
namespace val {
namespace ifnt {

    Entry::Entry(const Interest& interest)
        : m_nonce(interest.getNonce())
        , m_da(0)
    {
    }

    // copy constructor
    Entry::Entry(const Entry& entry)
        : m_nonce(entry.getNonce())
        , m_da(entry.getDA())
    {
    }

    Entry::~Entry()
    {
    }

    bool
    operator==(const Entry& l_entry, const Entry& r_entry)
    {
        return (l_entry.getNonce() == r_entry.getNonce() && l_entry.getDA() == r_entry.getDA());
    }

    bool
    operator==(std::shared_ptr<const Entry> l_entry, const Entry& r_entry)
    {
        return ((l_entry.get())->getNonce() == r_entry.getNonce() && (l_entry.get())->getDA() == r_entry.getDA());
    }

    bool
    operator==(std::unique_ptr<const Entry> l_entry, const Entry& r_entry)
    {
        return ((l_entry.get())->getNonce() == r_entry.getNonce() && (l_entry.get())->getDA() == r_entry.getDA());
    }

} // namespace ifnt 
} // namespace val
} // namespace ndn
} // namespace ns3