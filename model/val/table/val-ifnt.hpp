/**
 * jfp 2019
 * mieti - uminho
 */

/** \brief IFNT - Interest From Network Table, VAL support structure.
 * 
 * contains information of the LAL header and the nonce of every 
 * Interest that comes from the network. 
 */ 

#ifndef VAL_TABLE_IFNT_HPP
#define VAL_TABLE_IFNT_HPP

#include <boost/noncopyable.hpp>
#include <list>    // better for adding and removing elements
#include <vector>  // faster
#include <memory>  // for smart pointers

#include "val-ifnt-entry.hpp"


namespace ns3 {
namespace ndn {
namespace val {
namespace ifnt{

using Table = std::list<std::unique_ptr<Entry>>;
using ListMatchResult = std::vector<std::shared_ptr<const Entry>>;

class Ifnt : boost::noncopyable
{
public:

    /** \brief Default constructor
    */
    Ifnt();
    ~Ifnt();
    
    /** \brief Adds entry to IFNT
    *  \param entry a unique pointer tothe IFNT Entry
    *  \return true for success
    */
    void
    addEntry(Entry& entry);

    /** \brief removes entry from IFNT
     *  \param entry a reference to the IFNT Entry to remove
     *  \return true for success
     */ 
    bool
    removeEntry(const Entry& entry);

    /** \brief removes entry from IFNT by nonce
     *  \param nonce a uint32_t with the nonce value
     *  \return true for success
     */
    bool
    removeEntryByNonce(const uint32_t nonce) 
    {
        auto pair = findMatchByNonce(nonce);
        if(pair.second)
            return removeEntry(*pair.first.get()); //pointed element
        return pair.second;
    }

    /** \brief removes entries from IFNT by nonce list
     *  \param nonceList a pointer to the list of nonces
     *  \return true for success
     */
    bool
    removeEntriesByNonceList(std::vector<const uint32_t> *nonceList);

    /** \brief removes entry from IFNT by nonce and destination area
     *  \param nonce a uint32_t with the nonce value
     *  \param da a string with the destination area value
     *  \return true for success
     */
    bool
    removeEntryByNonceAndDA(const uint32_t nonce, const uint32_t da);

    /** \brief removes entries from IFNT by nonce list and destination area
     *  \param nonce a pointer for a list of nonces
     *  \param da a string with the destination area value
     *  \return true for success
     */
    bool
    removeEntriesByNonceListAndDA(std::vector<const uint32_t> *nonceList, const uint32_t da);

    /** \brief finds an entry in IFNT
     *  \param entry a reference to the IFNT entry to match
     *  \return pair in which the first member is a shared pointer to the entry
     *          and the second is a boolean refleting the succes of the operation 
     */
    std::pair<std::shared_ptr<const Entry>, bool>
    findMatch(const Entry& entry);

    /** \brief find an entry in IFNT by nonce
     *  \param nonce a uint32_t with the nonce value
     *  \return pair in which the first member is a shared pointer to the entry
     *          and the second is a boolean refleting the succes of the operation
     */
    std::pair<std::shared_ptr<const Entry>, bool>
    findMatchByNonce(const uint32_t nonce);

    /** \brief finds all the match in the IFNT given a list of nonces
     *  \param nonceList a pointer to the list of nonces
     *  \return a list of INFT entries shared pointers
     */
    ListMatchResult
    findMatchByNonceList(std::vector<const uint32_t> *nonceList);

    /** \brief finds all the match in the IFNT given a list of nonces and a destination area
     *  \param nonce a uint32_t with the nonce value
     *  \param da a string with the destination area information
     *  \return pair in which the first member is a shared pointer to the entry
     *          and the second is a boolean refleting the succes of the operation
     */
    std::pair<std::shared_ptr<const Entry>, bool>
    findMatchByNonceAndDA(const uint32_t nonce, const uint32_t da);


    /** \brief finds all the match in the IFNT given a list of nonces and a destination area
     *  \param nonceList a pointer to the list of nonces
     *  \param da a string with the destination area information
     *  \return a list of INFT entries shared pointers
     */
    ListMatchResult
    findMatchByNonceListandDA(std::vector<const uint32_t> *nonceList, const uint32_t da);

    inline Table*
    getTable()
    {
        return &m_table;
    }

    inline size_t
    getIfntSize()
    {
        return m_nItens;
    }

private:
    Table m_table;
    size_t m_nItens;
};


} // namespace ifnt
} // namespace val
} // namespace ndn
} // namespace ns3
#endif