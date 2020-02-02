/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_STRATEGY_HPP
#define VAL_STRATEGY_HPP

#include "val-forwarder.hpp"

#include "ns3/mobility-module.h"


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
    afterDfntHit(uint64_t faceId, const std::shared_ptr<const dfnt::Entry>& dfntEntry, ifnt::ListMatchResult* ifntEntries ,const ndn::Data& data)
    {
        this->doAfterDfntHit(faceId, dfntEntry, ifntEntries, data);
    }

    void
    afterDfntMiss(uint64_t faceId, const ndn::Data& data, ifnt::ListMatchResult* ifntEntries, bool isProducer)
    {
        this->doAfterDfntMiss(faceId, data, ifntEntries, isProducer);
    }

    void
    setMobilityModel(Ptr<ns3::WaypointMobilityModel> mobilityModel)
    {
        BOOST_ASSERT(mobilityModel != nullptr);
        if(mobilityModel != nullptr) {
            m_mobilityModel = mobilityModel;
        }
    }

protected:
    void
    sendValPacket(const uint64_t outFaceId, ValPacket& valPkt, time::nanoseconds duration)
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
            return pair.second->getGeoArea();
        }
        return "0";
    }

    u_int64_t
    getValNetFaceId()
    {
        auto it = m_valFwd.m_networkFaces.begin();
        return *it;
    }

    Ptr<ns3::WaypointMobilityModel>
    getMobilityModel()
    {
        return m_mobilityModel;
    }    

    virtual void
    doAfterIfntHit(uint64_t faceId, const std::shared_ptr<const ifnt::Entry>& ifntEntry, const ndn::Interest& interest) = 0;

    virtual void
    doAfterIfntMiss(uint64_t faceId, const ndn::Interest& interest) = 0;

    virtual void
    doAfterDfntHit(uint64_t faceId, const std::shared_ptr<const dfnt::Entry>& dfntEntry, ifnt::ListMatchResult* ifntEntries, const ndn::Data& data) = 0;

    virtual void
    doAfterDfntMiss(uint64_t faceId, const ndn::Data& data, ifnt::ListMatchResult* ifntEntries, bool isProducer) = 0;

protected:
    static constexpr long int MIN_INTEREST_WAIT = 32; // in ms
    static constexpr long int MAX_INTEREST_WAIT = 62; // in ms
    static constexpr long int MIN_DATA_WAIT = 0;      // in ms
    static constexpr long int MAX_DATA_WAIT = 30;     // in ms
    static constexpr long int ZERO_WAIT = 0;          // in ms
    static constexpr long int DELAY_IN_NANOS = 40000; // in ns
    static constexpr long int SIGNAL_RANGE = 110;      // in meters
    static constexpr long int AREA_SIZE = 200;        // area is a rectangle 200m x 200m (L x L);
    static constexpr long int MAX_DISTANCE = 5300;    // in meters

private:
    ValForwarder& m_valFwd;
    Ptr<ns3::WaypointMobilityModel> m_mobilityModel;
};



} // namespace val    
} // namespace ndn    
} // namespace ns3

#endif