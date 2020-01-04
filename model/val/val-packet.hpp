/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_VALPACKET_HPP
#define VAL_VALPACKET_HPP

#include "val-header.hpp"
#include <memory>

namespace ndn {
    class Interest;
    class Data;
} // namespace ndn


namespace ns3 {
namespace ndn {
namespace val {

class ValPacket
{
public:
    static const uint8_t NOT_SET = 0;
    static const uint8_t DATA_SET = 1;
    static const uint8_t INTEREST_SET = 2;

public:
    ValPacket(const ValHeader& valH);
    ~ValPacket();

    void
    setInterest(std::shared_ptr<::ndn::Interest> interest);

    void
    setData(std::shared_ptr<::ndn::Data> data);

    const ::ndn::Interest&
    getInterest() const;

    const ::ndn::Data&
    getData() const;

    const ValHeader&
    getValHeader() const;

    const uint8_t
    isSet() const; 

private:
    uint8_t m_isSet;
    const ValHeader& m_valH;
    std::shared_ptr<::ndn::Interest> m_interest;
    std::shared_ptr<::ndn::Data> m_data;
};


} // namespace val
} // namespace ndn
} // namespace ns3
#endif