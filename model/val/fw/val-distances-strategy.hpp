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
    doAfterDfntHit(uint64_t faceId, const std::shared_ptr<const dfnt::Entry>& dfntEntry, const ndn::Data& data) override;

    virtual void
    doAfterDfntMiss(uint64_t faceId, const ndn::Data& data, bool isProducer) override;

private:
    /* data */
};



} // namespace val    
} // namespace ndn    
} // namespace ns3

#endif