/**
 * jfp 2019
 * mieti - uminho
 */

#include "val-pft.hpp"
#include "../val-packet.hpp"

namespace ns3 {
namespace ndn {
namespace val {
namespace pft {

PFT::PFT()
    : m_nItens(0)
{
}

PFT::~PFT()
{
    m_pft.clear();
}

// add or find
std::pair<bool, Entry&>
PFT::addEntry(ValPacket&& valPkt)
{
    auto found = findMatch(valPkt);
    if(!found.first) {
        Entry entry(std::move(valPkt));
        m_pft.push_back(std::make_unique<Entry>(entry));
        m_nItens++;
        return {true, entry};
    }
    found.first = false;
    return found;
}


// it is equal if it has the same nonce, in the case of being an Interest Pkt
// it is equal if it has the same signature, in the case of being a Data Pkt
std::pair<bool, Entry&>
PFT::findMatch(const ValPacket& valPkt)
{
    auto it = m_pft.begin();
    for(; it != m_pft.end(); it++) {
        // it needs to be the same ndn type
        if(valPkt.isSet() == (*it)->getValPacket().isSet()) {
            if(valPkt.isSet() == ValPacket::INTEREST_SET) {
                if(valPkt.getInterest().getNonce() == (*it)->getValPacket().getInterest().getNonce()) {
                    return {true, **it};
                }
            } else if(valPkt.isSet() == ValPacket::DATA_SET) {
                if(valPkt.getData().getSignature() == (*it)->getValPacket().getData().getSignature()){
                    return {true, **it};
                }
            }
        }
    }
    return {false, **it};
}

bool
PFT::removeEntry(const ValPacket& valPkt)
{
    auto it = m_pft.begin();
    bool state = false;
    for(; it != m_pft.end(); it++) {
        // it needs to be the same ndn type
        if(valPkt.isSet() == (*it)->getValPacket().isSet()) {
            if(valPkt.isSet() == ValPacket::INTEREST_SET) {
                if(valPkt.getInterest().getNonce() == (*it)->getValPacket().getInterest().getNonce()) {
                    m_pft.erase(it);
                    state = true;
                    m_nItens--;
                    it = m_pft.end();
                }
            } else if(valPkt.isSet() == ValPacket::DATA_SET) {
                if(valPkt.getData().getSignature() == (*it)->getValPacket().getData().getSignature()){
                    m_pft.erase(it);
                    state = true;
                    m_nItens--;
                    it = m_pft.end();
                }
            }
        }
    }
    return state;
}

const size_t
PFT::getSize() const 
{
    return m_nItens;
}

} // namespace pft
} // namespace val
} // namespace ndn
} // namespace ns3