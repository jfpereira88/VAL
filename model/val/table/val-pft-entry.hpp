/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_PFT_ENTRY_HPP
#define VAL_PFT_ENTRY_HPP

#include "ns3/ndnSIM/NFD/core/scheduler.hpp"


namespace ns3 {
namespace ndn {
namespace val {
class ValPacket;
namespace pft {

using namespace ::ndn::literals::time_literals;
namespace time = ::ndn::time;

class Entry
{
friend class PFT;

public:
    // state
    static const int WAITING_FORWARDING = 0;
    static const int WAITING_IMPLICIT_ACK = 1;
    // tries
    static const int FORWARDING_TRIES = -1;
    static const int MAX_IMPLICIT_ACK_TRIES = 3;
    // static const time::milliseconds m_IMPLICIT_ACK_TIME = 100_ms;

public:
    Entry(ValPacket&& valPkt);
    ~Entry();

    // gets
    const ValPacket&
    getValPacket() const;

    const int
    getState() const;

    const int
    getNumberOfTries() const;

    const ::nfd::scheduler::EventId
    getTimerId() const;

    const time::milliseconds
    getDefaultImpAckTimerDuration() const;

    //sets
    bool
    changeStateToWaintingImpAck();

    bool
    oneLessTry();

    void
    setTimer(::nfd::scheduler::EventId timerId);

private:
    std::shared_ptr<ValPacket> m_valPkt;
    int m_state;
    int m_tries;
    ::nfd::scheduler::EventId m_timerId;
    // this should be static, but it complains
    const time::milliseconds m_IMPLICIT_ACK_TIME = 100_ms;
};



} // namespace pft
} // namespace val
} // namespace ndn
} // namespace ns3

#endif