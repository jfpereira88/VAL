/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_VALPACKET_HPP
#define VAL_VALPACKET_HPP

#include <ndn-cxx/encoding/block.hpp>

namespace ns3 {
namespace ndn {
namespace val {

using ::ndn::Block;

class ValPacket
{
public:
    ValPacket() = default;

    explicit
    ValPacket(Block& packet);

public:
    /** \brief the packet as a TLV block
     */
    Block valPkt;

};

}
}
}
#endif