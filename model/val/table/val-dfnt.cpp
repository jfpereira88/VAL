/**
 * jfp 2019
 * mieti - uminho
 */

#include"val-dfnt.hpp"

namespace ns3 {
namespace ndn {
namespace val {
namespace dfnt {

Dfnt::Dfnt()
    :m_nItens(0)
{
}

Dfnt::~Dfnt()
{
    m_table.clear();
}

void
Dfnt::addEntry(Entry& entry)
{
     m_table.push_back(std::make_unique<Entry>(std::move(entry)));
    m_nItens++;
}

bool 
Dfnt::removeEntry(const Entry& entry)
{
    std::list<std::unique_ptr<Entry>>::iterator it = m_table.begin();
    while (it != m_table.end())
    {
        if (*it->get() == entry)
        {
            m_table.erase(it);
            m_nItens--;
            return true;
        }
        it++;
    }
    return false;
}

bool
Dfnt::removeEntryBySignatureInfoAndSA(::ndn::SignatureInfo info, uint32_t sa)
{
    auto it = m_table.begin();
    while(it != m_table.end()) {
        if(it->get()->getSignatureInfo() == info && it->get()->getSA() == sa) {
            m_table.erase(it);
            m_nItens--;
            return true;
        }
    }
    return false;
}

std::shared_ptr<const Entry>
Dfnt::findMatch(::ndn::SignatureInfo info, uint32_t sa)
{
    auto it = m_table.begin();
    while(it != m_table.end()) {
        if(it->get()->getSignatureInfo() == info && it->get()->getSA() == sa) {
            return std::make_shared<const Entry>(*it->get());
        }
    }
    return nullptr;
}

} // namespace dfnt
} // namespace val
} // namespace ndn
} // namespace ns3