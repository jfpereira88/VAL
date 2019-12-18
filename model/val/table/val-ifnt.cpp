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

    void
    Ifnt::addEntry(Entry& entry)
    {
        m_table.push_back(std::make_unique<Entry>(std::move(entry)));
        m_nItens++;
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

    std::pair<std::shared_ptr<const Entry>, bool>
    Ifnt::findMatch(const Entry& entry)
    {
        std::list<std::unique_ptr<Entry>>::iterator it = m_table.begin();
        while (it != m_table.end())
        {
            if (*it->get() == entry)
                break;
            it++;
        }
        return std::make_pair(std::make_shared<const Entry>(entry), it != m_table.end()); //best if deleted on the caller
       
    }


} // namespace ifnt
} // namespcae val
} // namespcace ndn
} // namespcace ns3