/**
 * jfp 2019
 * mieti - uminho
 */


#include "val-distances-strategy.hpp"

namespace ns3 {
namespace ndn {
namespace val {


ValDistancesStrategy::ValDistancesStrategy(ValForwarder& valFwd)
    : ValStrategy(valFwd)
{
}

ValDistancesStrategy::~ValDistancesStrategy() = default;

void
ValDistancesStrategy::doAfterIfntHit(uint64_t faceId, const std::shared_ptr<const ifnt::Entry>& ifntEntry, const ndn::Interest& interest)
{
    //if(ifntEntry->getDA() != "0") { // known destination
        // get distance between ifntEntry->getPhPos() and ifntEntry->getDA()
        // preHopDist = getDistance(ifntEntry->getPhPos(), ifntEntry->getDA());
        // get distance between getMyPos() and ifntEntry->getDA()
        // myDist = getDistance(getMyPos(), ifntEntry->getDA());
        // if(myDist < preHopDist) {
        //      construct ValHeader that respects consumer's will
        //      ValHeader valH(ifntEntry->getSA(), ifntEntry->getDA(), 
        //            getMyPos(), ifntEntry->getRN(), ifntEntry->getHopC());
        //      ValPacket valP(valH);
        //      valP.setInterest(std::make_shared<Interest>(interest));
        //      calculate the duration of the forwarding timer, less distance less time
        //      time::milliseconds time = calcForwardingTimer(myDist);
        //      sendValPacket(ifnt->getFaceId(), valP, time);
        // } else {
        //      drop packet   
        // }
    //} else {  // exploration phase
        // get distance betwwen the current node and the previous node
        // dist = getDistance(ifntEntry->getPhPos(), getMyPos());
        // calculate the duration of the forwarding timer, more distance less time
        // time::milliseconds time = calcInvertedForwardingTimer(dist);
        // std::string destinationArea = this->getGeoArea(faceId);
        //  ValHeader valH(ifntEntry->getSA(), destinationArea, 
        //            getMyPos(), ifntEntry->getRN(), ifntEntry->getHopC());
        //  ValPacket valP(valH);
        //  valP.setInterest(std::make_shared<Interest>(interest));
        //  sendValPacket(ifnt->getFaceId(), valP, time);
    //}
    ValHeader valH(ifntEntry->getSA(), ifntEntry->getDA(), 
                    ifntEntry->getPhPos(), ifntEntry->getRN(), ifntEntry->getHopC());
    ValPacket valP(valH);
    valP.setInterest(std::make_shared<Interest>(interest));
}

void
ValDistancesStrategy::doAfterIfntMiss(uint64_t faceId, const ndn::Interest& interest)
{

}

void
ValDistancesStrategy::doAfterDfntHit(uint64_t faceId, const std::shared_ptr<const dfnt::Entry>& dfntEntry, const ndn::Data& data)
{

}

void
ValDistancesStrategy::doAfterDfntMiss(uint64_t faceId, const ndn::Data& data, bool isProducer)
{

}

} // namespace val    
} // namespace ndn    
} // namespace ns3
