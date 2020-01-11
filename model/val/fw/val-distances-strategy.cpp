/**
 * jfp 2019
 * mieti - uminho
 */


#include "val-distances-strategy.hpp"
#include <ndn-cxx/util/random.hpp>
#include <boost/chrono.hpp>
#include "ns3/log.h"



NS_LOG_COMPONENT_DEFINE("ndn.val.ValDistancesStrategy");


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
    if(ifntEntry->getHopC() == 0) {
        return; // drop packet
    }
    /*
    if(ifntEntry->getDA() != "0") { // known destination
        // get distance between ifntEntry->getPhPos() and ifntEntry->getDA()
        uint32_t preHopDist = getDistanceToArea(ifntEntry->getPhPos(), ifntEntry->getDA());
        // get distance between getMyPos() and ifntEntry->getDA()
        uint32_t myDist = getDistanceToArea(getMyPos(), ifntEntry->getDA());
        if(myDist < preHopDist) {
        //    construct ValHeader that respects consumer's will
              ValHeader valH(ifntEntry->getSA(), ifntEntry->getDA(), 
                    getMyPos(), ifntEntry->getRN(), ifntEntry->getHopC());
              ValPacket valP(valH);
              valP.setInterest(std::make_shared<Interest>(interest));
        //      calculate the duration of the forwarding timer, less distance less time
              time::milliseconds time = calcFwdTimer(myDist);
              sendValPacket(ifntEntry->getFaceId(), valP, time);
         } else {
        //      drop packet   
         }
    } else {  // exploration phase
        // get distance betwwen the current node and the previous node
        uint32_t dist = getDistanceToPoint(ifntEntry->getPhPos(), getMyPos());
        // calculate the duration of the forwarding timer, more distance less time
        time::milliseconds time = calcInvertedFwdTimer(dist);
        std::string destinationArea = this->getGeoArea(faceId);
        ValHeader valH(ifntEntry->getSA(), destinationArea, 
                getMyPos(), ifntEntry->getRN(), ifntEntry->getHopC());
        ValPacket valP(valH);
        valP.setInterest(std::make_shared<Interest>(interest));
        sendValPacket(ifntEntry->getFaceId(), valP, time);
    }
    */
    ValHeader valH(ifntEntry->getSA(), ifntEntry->getDA(), 
                    ifntEntry->getPhPos(), ifntEntry->getRN(), ifntEntry->getHopC());
    ValPacket valP(valH);
    valP.setInterest(std::make_shared<Interest>(interest));
    time::milliseconds time = this->DEFAULT_INTEREST_WAIT;
    time = time + time::duration_cast<time::milliseconds>(generateMicroSecondDelay());
    NS_LOG_DEBUG("Timer: " << time);
    sendValPacket(ifntEntry->getFaceId(), valP, time);
}

void
ValDistancesStrategy::doAfterIfntMiss(uint64_t faceId, const ndn::Interest& interest)
{
    /*
    std::string destinationArea = this->getGeoArea(faceId);
    uint8_t hopC = ValHeader::MAXHOPS;
    time::milliseconds time = this->DEFAULT_INTEREST_WAIT;
    ValHeader valH(getMyArea(), destinationArea, 
                getMyPos(), interest.getName().toUri(), hopC);
    ValPacket valP(valH);
    valP.setInterest(std::make_shared<Interest>(interest));
    sendValPacket(getValNetFaceId(), valP, time);
    */

    std::string destinationArea = this->getGeoArea(faceId);
    uint8_t hopC = ValHeader::MAXHOPS;
    time::milliseconds time = this->DEFAULT_INTEREST_WAIT;
    ValHeader valH("0", destinationArea, 
                "0", interest.getName().toUri(), hopC);
    ValPacket valP(valH);
    valP.setInterest(std::make_shared<Interest>(interest));
    sendValPacket(getValNetFaceId(), valP, time);
}

void
ValDistancesStrategy::doAfterDfntHit(uint64_t faceId, const std::shared_ptr<const dfnt::Entry>& dfntEntry, ifnt::ListMatchResult* ifntEntries, const ndn::Data& data)
{
    if(dfntEntry->getHopC() == 0) {
        return; // drop packet
    }
    std::vector<std::string> nextHopsPosList = getPositions(ifntEntries);
    uint32_t prevHopDist = getMultiPointDist(dfntEntry->getPhPos(), &nextHopsPosList);
    uint32_t myDist = getMultiPointDist(getMyPos(), &nextHopsPosList);
    if(myDist < prevHopDist) {
        time::milliseconds time = calcFwdTimer(myDist);
        ValHeader valH(dfntEntry->getSA(), dfntEntry->getDA(), 
                    getMyPos(), dfntEntry->getRN(), dfntEntry->getHopC());
        ValPacket valP(valH);
        valP.setData(std::make_shared<Data>(data));
        sendValPacket(dfntEntry->getFaceId(), valP, time);
        // @REMEMBER: Data Last hop does not receive ImpACK
    }
    // else drop packet
}

void
ValDistancesStrategy::doAfterDfntMiss(uint64_t faceId, const ndn::Data& data, ifnt::ListMatchResult* ifntEntries, bool isProducer)
{
    auto pair = getLongestJorney(ifntEntries);
    uint8_t hopc = ValHeader::MAXHOPS - pair.first + 1; // one more hop for good luck
    std::string srcArea;
    if(isProducer) {
        srcArea = getMyArea();
    } else {
        srcArea = this->getGeoArea(faceId);
    }
    ValHeader valH(srcArea, pair.second, 
                getMyPos(), data.getName().toUri(), hopc);
    ValPacket valP(valH);
    valP.setData(std::make_shared<Data>(data));
    time::milliseconds time = this->DEFAULT_DATA_WAIT;
    sendValPacket(getValNetFaceId(), valP, time);
    // @REMEMBER: Data Last hop does not receive ImpACK
}

time::microseconds
ValDistancesStrategy::generateMicroSecondDelay()
{
    long int random = ::ndn::random::generateWord32();
    random = random / (ValDistancesStrategy::MAX_32WORD_RANDOM/ValDistancesStrategy::DELAY_IN_MICROS);
    return time::microseconds{random};
}

std::vector<std::string>
ValDistancesStrategy::getPositions(ifnt::ListMatchResult* ifntEntriesList)
{
    std::vector<std::string> res;
    for(auto it = ifntEntriesList->begin(); it != ifntEntriesList->end(); it++) {
        res.push_back((*it)->getPhPos());
    }
    return res;
}


uint32_t
ValDistancesStrategy::getMultiPointDist(const std::string pointA, std::vector<std::string> *pointsList)
{
    return 10;
}

uint32_t
ValDistancesStrategy::getDistanceToArea(const std::string pointA, const std::string area)
{
    return 10;
}

uint32_t
ValDistancesStrategy::getDistanceToPoint(const std::string pointA, const std::string pointB)
{
    return 10;
}

std::string
ValDistancesStrategy::getMyPos()
{
    return "0";
}

std::string
ValDistancesStrategy::getMyArea()
{
    return "0";
}

time::milliseconds
ValDistancesStrategy::calcFwdTimer(uint32_t dist)
{
 return 0_ms;
}

time::milliseconds
ValDistancesStrategy::calcInvertedFwdTimer(uint32_t dist)
{
    return 0_ms;
}

std::pair<uint32_t, std::string>
ValDistancesStrategy::getLongestJorney(ifnt::ListMatchResult* ifntEntriesList)
{
    auto it = ifntEntriesList->begin();
    uint8_t min = (*it)->getHopC();
    std::string srcArea = (*it)->getSA(); 
    it++;  // next
    while(it != ifntEntriesList->end()) {
        if(min > (*it)->getHopC()) {
            min = (*it)->getHopC();
            srcArea = (*it)->getSA();
        }
        it++;
    }
    return {min, srcArea};
}

} // namespace val    
} // namespace ndn    
} // namespace ns3
