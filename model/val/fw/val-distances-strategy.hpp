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
private:
static constexpr uint16_t DELAY_IN_MICROS = 4000;
static constexpr long int MAX_32WORD_RANDOM = 4294967296;

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

    time::microseconds
    generateMicroSecondDelay();

    std::vector<std::string>
    getPositions(ifnt::ListMatchResult* ifntEntriesList);

    uint32_t
    getMultiPointDist(const std::string pointA, std::vector<std::string> *pointsList);

    uint32_t
    getDistanceToArea(const std::string pointA, const std::string area);

    uint32_t
    getDistanceToPoint(const std::string pointA, const std::string pointB);

    std::string
    getMyPos();

    std::string
    getMyArea();

    time::milliseconds
    calcFwdTimer(uint32_t dist);

    time::milliseconds
    calcInvertedFwdTimer(uint32_t dist);

    std::pair<uint32_t, std::string>
    getLongestJorney(ifnt::ListMatchResult* ifntEntriesList);

private:
    /* data */
};



} // namespace val    
} // namespace ndn    
} // namespace ns3

#endif