/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_STRATEGY_HPP
#define VAL_STRATEGY_HPP

#include "val-forwarder.hpp"

namespace ns3 {
namespace ndn {
namespace val {

class ValStrategy
{
public:
    ValStrategy(ValForwarder& valFwd);

    virtual
    ~ValStrategy();

    void
    afterIfntHit(uint64_t faceId, const std::shared_ptr<const ifnt::Entry>& ifntEntry, const ndn::Interest& interest)
    {
        this->doAfterIfntHit(faceId, ifntEntry, interest);
    }

    void
    afterIfntMiss(uint64_t faceId, const ndn::Interest& interest)
    {
        this->doAfterIfntMiss(faceId, interest);
    }

    void
    afterDfntHit(uint64_t faceId, const std::shared_ptr<const dfnt::Entry>& dfntEntry, const ndn::Data& data)
    {
        this->doAfterDfntHit(faceId, dfntEntry, data);
    }

    void
    afterDfntMiss(uint64_t faceId, const ndn::Data& data, bool isProducer)
    {
        this->doAfterDfntMiss(faceId, data, isProducer);
    }

protected:
    void
    sendValPacket(const uint64_t outFaceId, const ValPacket& valPkt, time::milliseconds duration)
    {
        m_valFwd.registerOutgoingValPacket(outFaceId, valPkt, duration);
    }

    const f2a::F2A&
    getF2A()
    {
        return m_valFwd.m_f2a;
    }

    const std::string
    getGeoArea(uint64_t faceId)
    {
        auto pair = m_valFwd.m_f2a.findByFaceId(faceId);
        if(pair.first) {
            return pair.second.getGeoArea();
        }
        return "0";
    }

    virtual void
    doAfterIfntHit(uint64_t faceId, const std::shared_ptr<const ifnt::Entry>& ifntEntry, const ndn::Interest& interest) = 0;

    virtual void
    doAfterIfntMiss(uint64_t faceId, const ndn::Interest& interest) = 0;

    virtual void
    doAfterDfntHit(uint64_t faceId, const std::shared_ptr<const dfnt::Entry>& dfntEntry, const ndn::Data& data) = 0;

    virtual void
    doAfterDfntMiss(uint64_t faceId, const ndn::Data& data, bool isProducer) = 0;

private:
    ValForwarder& m_valFwd;
};



} // namespace val    
} // namespace ndn    
} // namespace ns3

#endif