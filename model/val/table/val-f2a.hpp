/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_F2A_TABLE_HPP
#define VAL_F2A_TABLE_HPP

#include <boost/noncopyable.hpp>
#include <list>    // better for adding and removing elements
#include <memory>  // for smart pointers

#include "val-f2a-entry.hpp"

namespace ns3 {
namespace ndn {
namespace val {
namespace f2a {

using Table = std::list<std::unique_ptr<Entry>>; 

class F2A
{
public:
    F2A();
    ~F2A();

    void
    addEntry(Entry&& entry);

    void
    removeEntry(Entry& entry);

    void
    removeByFaceId(uint64_t faceId);

    void
    removeByGeoArea(std::string geoArea);

    const bool
    findEntry(Entry& entry);

    std::pair<bool, const Entry&>
    findByFaceId(uint64_t faceId);

    std::pair<bool, const Entry&>
    findByGeoArea(std::string geoArea);

    const size_t
    getSize() const;

private:
    Table m_f2a;
    size_t m_size;
};




} // namespace f2a
} // namespace val
} // namespace ndn
} // namespace ns3

#endif