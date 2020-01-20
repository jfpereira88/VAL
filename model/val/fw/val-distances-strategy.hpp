/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_DISTANCES_STRATEGY_HPP
#define VAL_DISTANCES_STRATEGY_HPP

#include "val-strategy.hpp"

namespace ns3 {
namespace ndn {
namespace val {

class ValDistancesStrategy : public ValStrategy
{
public:
    ValDistancesStrategy(ValForwarder& valFwd);
    ~ValDistancesStrategy();

private:
    virtual void
    doAfterIfntHit(uint64_t faceId, const std::shared_ptr<const ifnt::Entry>& ifntEntry, const ndn::Interest& interest) override;

    virtual void
    doAfterIfntMiss(uint64_t faceId, const ndn::Interest& interest) override;

    virtual void
    doAfterDfntHit(uint64_t faceId, const std::shared_ptr<const dfnt::Entry>& dfntEntry, ifnt::ListMatchResult* ifntEntries, const ndn::Data& data) override;

    virtual void
    doAfterDfntMiss(uint64_t faceId, const ndn::Data& data, ifnt::ListMatchResult* ifntEntries, bool isProducer) override;

    time::nanoseconds
    generateMicroSecondDelay();

    std::vector<std::string>
    getPositions(ifnt::ListMatchResult* ifntEntriesList);

    double
    getMultiPointDist(const std::string pointA, std::vector<std::string> *pointsList);

    double
    getDistanceToArea(const std::string pointA, const std::string area);

    double
    getDistanceToPoint(const std::string pointA, const std::string pointB);

    std::string
    getMyPos();

    std::string
    getMyArea();

    time::nanoseconds
    calcFwdTimer(double dist, uint8_t hopC = 0, bool toArea = false, bool isData = false);

    time::nanoseconds
    calcInvertedFwdTimer(double dist, uint8_t hopC = 0, bool toArea = false, bool isData = false);

    std::string
    getAreaFromPosition(double _x, double _y);

    ns3::Vector3D
    getPositionFromArea(std::string area);

    /**
     *  \brief return the information of the interest that as done the longest jorney
     *  \return first: hopc, second: sourceArea
     */
    std::pair<uint8_t, std::string>
    getLongestJorney(ifnt::ListMatchResult* ifntEntriesList);

private:
    Ptr<UniformRandomVariable> m_randomNum;
};



} // namespace val    
} // namespace ndn    
} // namespace ns3

#endif