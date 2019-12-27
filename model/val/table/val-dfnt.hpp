/**
 * jfp 2019
 * mieti - uminho
 */
#ifndef VAL_TABLE_DFNT_HPP
#define VAL_TABLE_DFNT_HPP

#include <boost/noncopyable.hpp>
#include <list>    // better for adding and removing elements
#include <memory>  // for smart pointers

#include "val-dfnt-entry.hpp"

namespace ns3 {
namespace ndn {
namespace val {
namespace dfnt {

using Table = std::list<std::unique_ptr<Entry>>; 

class Dfnt : boost::noncopyable
{
public:
    Dfnt();
    ~Dfnt();

    /** \brief Adds entry to DFNT
    *  \param entry a DFNT Entry
    *  \return true for success
    */
    void
    addEntry(Entry& entry);

    /** \brief removes entry from DFNT
     *  \param entry a reference to the DFNT Entry to remove
     *  \return true for success
     */ 
    bool
    removeEntry(const Entry& entry);

    bool
    removeEntryBySignatureInfoAndSA(::ndn::SignatureInfo info, uint32_t sa);

    std::shared_ptr<const Entry>
    findMatch(::ndn::SignatureInfo info, uint32_t sa);

    inline size_t
    getDfntSize()
    {
        return m_nItens;
    }

private:
    Table m_table;
    size_t m_nItens;
};


} // namespace dfnt
} // namespace val
} // namespace ndn
} // namespace ns3    

#endif