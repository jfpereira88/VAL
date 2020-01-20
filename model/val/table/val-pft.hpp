/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_PFT_HPP
#define VAL_PFT_HPP

#include "val-pft-entry.hpp"

namespace ns3 {
namespace ndn {
namespace val {
class ValPacket;
namespace pft {

using Table = std::list<std::shared_ptr<Entry>>;
using ListMatchResult = std::vector<std::shared_ptr<Entry>>;

class PFT
{
    
public:
    PFT(/* args */);
    ~PFT();

    std::pair<bool, std::shared_ptr<Entry>>
    addEntry(Entry& entry);

    std::pair<bool, std::shared_ptr<Entry>>
    findMatch(const ValPacket& valPkt);

    ListMatchResult
    findMatchByNonceList(std::vector<uint32_t> *nonceList);

    bool
    removeEntry(const ValPacket& valPkt);

    void
    removeEntriesByMatchList(ListMatchResult& entries)
    {
        for(const auto& entry : entries) {
            removeEntry(entry->getValPacket());
        }
    }

    const size_t
    getSize() const; 

private:
    Table m_pft;
    size_t m_nItens;
};







} // namespace pft
} // namespace val
} // namespace ndn
} // namespace ns3

#endif