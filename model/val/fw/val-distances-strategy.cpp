/**
 * jfp 2019
 * mieti - uminho
 */


#include "val-distances-strategy.hpp"
#include <ndn-cxx/util/random.hpp>
#include <boost/chrono.hpp>
#include "ns3/log.h"

#include "ns3/vector.h"



NS_LOG_COMPONENT_DEFINE("ndn.val.ValDistancesStrategy");


namespace ns3 {
namespace ndn {
namespace val {


ValDistancesStrategy::ValDistancesStrategy(ValForwarder& valFwd)
    : ValStrategy(valFwd)
    , m_randomNum(CreateObject<UniformRandomVariable>())
{
}

ValDistancesStrategy::~ValDistancesStrategy() = default;

void
ValDistancesStrategy::doAfterIfntHit(uint64_t faceId, const std::shared_ptr<const ifnt::Entry>& ifntEntry, const ndn::Interest& interest)
{
    if(ifntEntry->getHopC() == 0) {
        return; // drop packet
    }
    
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
    /*
    ValHeader valH(ifntEntry->getSA(), ifntEntry->getDA(), 
                    ifntEntry->getPhPos(), ifntEntry->getRN(), ifntEntry->getHopC());
    ValPacket valP(valH);
    valP.setInterest(std::make_shared<Interest>(interest));
    time::milliseconds time = time::milliseconds{ValDistancesStrategy::MIN_INTEREST_WAIT};
    time = time + time::duration_cast<time::milliseconds>(generateMicroSecondDelay());
    NS_LOG_DEBUG("Timer: " << time);
    sendValPacket(ifntEntry->getFaceId(), valP, time);
    */
}

void
ValDistancesStrategy::doAfterIfntMiss(uint64_t faceId, const ndn::Interest& interest)
{
    
    std::string destinationArea = this->getGeoArea(faceId);
    uint8_t hopC = ValHeader::MAXHOPS;
    time::milliseconds time = time::milliseconds{ValDistancesStrategy::ZERO_WAIT};
    ValHeader valH(getMyArea(), destinationArea, 
                getMyPos(), interest.getName().toUri(), hopC);
    ValPacket valP(valH);
    valP.setInterest(std::make_shared<Interest>(interest));
    sendValPacket(getValNetFaceId(), valP, time);
    
    /*
    std::string destinationArea = this->getGeoArea(faceId);
    uint8_t hopC = ValHeader::MAXHOPS;
    time::milliseconds time = time::milliseconds{ValDistancesStrategy::MIN_INTEREST_WAIT};
    ValHeader valH("0", destinationArea, 
                "0", interest.getName().toUri(), hopC);
    ValPacket valP(valH);
    valP.setInterest(std::make_shared<Interest>(interest));
    sendValPacket(getValNetFaceId(), valP, time);
    */
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
        time::milliseconds time = calcFwdTimer(myDist, true);
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
    time::milliseconds time = time::milliseconds{ValDistancesStrategy::ZERO_WAIT};
    sendValPacket(getValNetFaceId(), valP, time);
    // @REMEMBER: Data Last hop does not receive ImpACK
}

time::microseconds
ValDistancesStrategy::generateMicroSecondDelay()
{
    //long int random = ::ndn::random::generateWord32();
    //random = random / (ValDistancesStrategy::MAX_32WORD_RANDOM/ValDistancesStrategy::DELAY_IN_MICROS);
    long int random = m_randomNum->GetValue(0, ValDistancesStrategy::DELAY_IN_MICROS);
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


double
ValDistancesStrategy::getMultiPointDist(const std::string pointA, std::vector<std::string> *pointsList)
{
    size_t nItens = pointsList->size();
    // geting first vector;
    std::stringstream stream_point;
    stream_point << pointA;
    ns3::Vector3D vectorA;
    stream_point >> vectorA;

    // reset stream
    stream_point.str(std::string());

    double dist = 0;
    for(std::string point : *pointsList) {
        stream_point << point;
        ns3::Vector3D vector;
        stream_point >> vector;
        dist += ns3::CalculateDistance(vectorA, vector);
        // reset stream
        stream_point.str(std::string());
    }
    // calculate the mean
    dist = dist / double(nItens);
    return dist;
}

double
ValDistancesStrategy::getDistanceToArea(const std::string pointA, const std::string area)
{
    std::stringstream stream_pointA;
    stream_pointA << pointA;
    ns3::Vector3D vectorA;
    stream_pointA >> vectorA;
    return ns3::CalculateDistance(vectorA, getPositionFromArea(area));
}

double
ValDistancesStrategy::getDistanceToPoint(const std::string pointA, const std::string pointB)
{
    std::stringstream stream_pointA;
    std::stringstream stream_pointB;
    stream_pointA << pointA;
    stream_pointB << pointB;

    ns3::Vector3D vectorA;
    ns3::Vector3D vectorB;

    stream_pointA >> vectorA;
    stream_pointB >> vectorB;
    
    return ns3::CalculateDistance(vectorA, vectorB);
}

std::string
ValDistancesStrategy::getMyPos()
{
    std::stringstream stream;
    ns3::Vector3D myPosVector = this->getMobilityModel()->GetPosition();
    stream << myPosVector;
    return stream.str();
}

std::string
ValDistancesStrategy::getMyArea()
{
    ns3::Vector3D myPosVector = this->getMobilityModel()->GetPosition();
    // check X and Y agains the pseudo AreaTable
    return getAreaFromPosition(myPosVector.x, myPosVector.y);
}

time::milliseconds
ValDistancesStrategy::calcFwdTimer(double dist, bool isData)
{ 
    // less distance less time;
    double time = 0.0;
    if(isData) { // uses prev Hop position
        double dataWaitRange(ValDistancesStrategy::MAX_DATA_WAIT);
        double comunicationRange(ValDistancesStrategy::SIGNAL_RANGE * 2);
        time = dist / (comunicationRange / dataWaitRange);
    } else { // interest uses DA
        double interestWaitRange(ValDistancesStrategy::MAX_INTEREST_WAIT - ValDistancesStrategy::MAX_INTEREST_WAIT);
        double biggestDistance(ValDistancesStrategy::MAX_DISTANCE);
        time = dist / (biggestDistance / interestWaitRange);
    }
    
    // getting the decimal part and convert it to microseconds
    double whole;
    double frac = std::modf(time, &whole);
    frac = frac * 100; // micros - shiffting coma, now we have micros
    time::microseconds micros = time::microseconds{int(frac)};
    // getting the integer part and convert it to millisecons
    time::milliseconds millis = time::milliseconds{int(whole)};
    time::milliseconds duration;
    if(!isData){
        duration = time::milliseconds{ValDistancesStrategy::MIN_INTEREST_WAIT} + 
                    millis + time::duration_cast<time::milliseconds>(micros) + 
                    time::duration_cast<time::milliseconds>(generateMicroSecondDelay());
    } else {
        duration = millis + time::duration_cast<time::milliseconds>(micros) + 
                    time::duration_cast<time::milliseconds>(generateMicroSecondDelay());
    }
    return duration;
}

time::milliseconds
ValDistancesStrategy::calcInvertedFwdTimer(double dist, bool isData)
{
    // less distance less time;
    double time = 0.0;
    double comunicationRange(ValDistancesStrategy::SIGNAL_RANGE * 2);
    double timeInterval = 0.0;
    if(isData) { // uses prev Hop position
        timeInterval = double(ValDistancesStrategy::MAX_DATA_WAIT);
    } else { // interest uses prev hop position
        timeInterval = double(ValDistancesStrategy::MAX_INTEREST_WAIT - ValDistancesStrategy::MAX_INTEREST_WAIT);
    }
    time = (1.0 / dist) / (comunicationRange / timeInterval);
    
    // getting the decimal part and convert it to microseconds
    double whole;
    double frac = std::modf(time, &whole);
    frac = frac * 100; // micros - shiffting coma, now we have micros
    time::microseconds micros = time::microseconds{int(frac)};
    // getting the integer part and convert it to millisecons
    time::milliseconds millis = time::milliseconds{int(whole)};
    time::milliseconds duration;
    if(!isData){
        duration = time::milliseconds{ValDistancesStrategy::MIN_INTEREST_WAIT} + 
                    millis + time::duration_cast<time::milliseconds>(micros) + 
                    time::duration_cast<time::milliseconds>(generateMicroSecondDelay());
    } else {
        duration = millis + time::duration_cast<time::milliseconds>(micros) + 
                    time::duration_cast<time::milliseconds>(generateMicroSecondDelay());
    }
    return duration;
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

std::string
ValDistancesStrategy::getAreaFromPosition(double _x, double _y)
{
    double temp_x = _x / ValDistancesStrategy::AREA_SIZE;
    double temp_y = _y / ValDistancesStrategy::AREA_SIZE;

    // round down (truncate) doubles
    int area_x_step = int(temp_x);
    int area_y_step = int(temp_y);

    std::stringstream stream;

    char origin_x = 'A';  // ASCII 65
    char origin_y = '1';  // ASCII 49
    // set bounderies
    if(area_x_step > 25)
        origin_x = 'Z';
    else
        origin_x += area_x_step;
    
    if(area_y_step > 5)
        origin_y = '5';
    else
        origin_y += area_y_step;
        
        
    stream << origin_x << origin_y;
    return stream.str();
}

ns3::Vector3D
ValDistancesStrategy::getPositionFromArea(std::string area)
{
    auto it = area.begin();
    int first = *it;
    int second = *++it;
    double dist_x = 100.0;  // default area A1
    double dist_y = 100.0;  // default area A1
    int step_x = first - 65;  // 65 ASCII A
    int step_y = second - 49; // 49 ASCII 1

    // the positon of the middle of the area
    if(step_x >= 0) {
        dist_x = (ValDistancesStrategy::AREA_SIZE * (step_x +1)) - (ValDistancesStrategy::AREA_SIZE / 2);
    }
    if(step_y >= 0) {
        dist_y = (ValDistancesStrategy::AREA_SIZE * (step_y +1)) - (ValDistancesStrategy::AREA_SIZE / 2);
    }
    return ns3::Vector3D(dist_x, dist_y, 0.0);
}

} // namespace val    
} // namespace ndn    
} // namespace ns3
