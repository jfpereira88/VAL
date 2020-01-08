/**
 * jfp 2019
 * mieti - uminho
 */

#include "val-ifnt.hpp"

namespace ns3 {
namespace ndn {
namespace val {
namespace ifnt {

    Ifnt::Ifnt()
        :m_nItens(0)
    {
    }

    Ifnt::~Ifnt()
    {
        m_table.clear();
    }

    bool
    Ifnt::addEntry(Entry& entry)
    {
        auto pair = findMatch(entry);
        if(!pair.first) {
            m_table.push_back(std::make_unique<Entry>(std::move(entry)));
            m_nItens++;
        }
        return !pair.first;
    }
    
    bool 
    Ifnt::removeEntry(const Entry& entry)
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

    std::pair<bool, std::shared_ptr<const Entry>>
    Ifnt::findMatch(const Entry& entry)
    {
        std::list<std::unique_ptr<Entry>>::iterator it = m_table.begin();
        while (it != m_table.end())
        {
            if (**it == entry)
                break;
            it++;
        }
        return std::make_pair(it != m_table.end(), std::make_shared<const Entry>(entry)); //best if deleted on the caller
       
    }

    std::pair<bool, std::shared_ptr<const Entry>>
    Ifnt::findMatchByNonce(const uint32_t nonce)
    {
        for (auto it = m_table.begin(); it != m_table.end(); it++) {
            if(it->get()->getNonce() == nonce)
                return {it != m_table.end(), std::make_shared<const Entry>(**it)};
        }
        return {false, nullptr};
    }
/*
    ListMatchResult
    Ifnt::findMatchByNonceList(std::vector<const uint32_t> *nonceList)
    {
        ListMatchResult result;
        for (uint32_t nonce : *nonceList) {
            auto pair = findMatchByNonce(nonce);
            if(pair.first)
                result.push_back(pair.second);
        }
        return result;
    }
*/

} // namespace ifnt
} // namespcae val
} // namespcace ndn
} // namespcace ns3