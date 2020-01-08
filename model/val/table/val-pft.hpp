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

using Table = std::list<std::unique_ptr<Entry>>; 

class PFT
{
    
public:
    PFT(/* args */);
    ~PFT();

    std::pair<bool, Entry&>
    addEntry(ValPacket&& valPkt);

    std::pair<bool, Entry&>
    findMatch(const ValPacket& valPkt);

    bool
    removeEntry(const ValPacket& valPkt);

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