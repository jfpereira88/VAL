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
Dfnt::addEntry(Entry&& entry)
{
    m_table.push_back(std::make_unique<Entry>(entry));
    m_nItens++;
}

bool 
Dfnt::removeEntry(const Entry& entry)
{
    std::list<std::unique_ptr<Entry>>::iterator it = m_table.begin();
    while (it != m_table.end())
    {
        if (**it == entry)
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
Dfnt::removeEntryBySignatureAndSA(::ndn::Signature sig, std::string sa)
{
    auto it = m_table.begin();
    while(it != m_table.end()) {
        if((*it)->getSignature() == sig && (*it)->getSA() == sa) {
            m_table.erase(it);
            m_nItens--;
            return true;
        }
        it++;
    }
    return false;
}

std::pair<bool, const Entry&>
Dfnt::findMatch(::ndn::Signature sig, std::string sa)
{
    auto it = m_table.begin();
    while(it != m_table.end()) {
        if((*it)->getSignature() == sig && (*it)->getSA() == sa) {
            return {true, **it};
        }
        it++;
    }
    return {false, **it};
}

} // namespace dfnt
} // namespace val
} // namespace ndn
} // namespace ns3