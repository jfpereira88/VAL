/**
 * \mainpage
 * This work introduces an application of the Named Data Networking (NDN) to Vehicular 
 * Ad-Hoc Networks (VANETs), with support for geographical-based forwarding. 
 * The initial design, dubbed Vehicle Adaptation Layer (VAL), illustrates how this can 
 * be done without altering the NDN primitives and core structures. 
 * The design provides (1) a mechanism to bind NDN data names to the producers’ 
 * and consumers' geographic areas; (2) an algorithm to guide Interests towards data 
 * producers using the distances between nodes and distances to the producers' areas;  
 * (3) an algorithm to guide Data packets towards data consumers' using the distances 
 * between nodes; (4) a discovery mechanism to find producers' areas.
 * 
 * Masters Dissertion:
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_COMMON_HPP
#define VAL_COMMON_HPP

#include <ndn-cxx/util/time.hpp>
#include <ndn-cxx/encoding/block.hpp>
#include <ndn-cxx/signature.hpp>
#include <ndn-cxx/util/signal.hpp>
#include <ndn-cxx/data.hpp>
#include <ndn-cxx/interest.hpp>

#include <memory>

#include "ns3/ndnSIM/NFD/daemon/face/face.hpp"


namespace ns3 {
namespace ndn {
namespace val {

namespace ndn {
    using namespace ::ndn;
} // namespace ndn

namespace time {
    using namespace ::ndn::time;
} // namespace time

namespace lp {
    using namespace ::ndn::lp;
} // namespace lp

namespace tlv {
    using namespace ::ndn::tlv;
} // namespace tlv

namespace signal {
    using namespace ::ndn::util::signal;
} 

namespace nfd {
    using namespace ::nfd;
} // namespace nfd

using std::shared_ptr;
using std::make_shared;

using ::ndn::Interest;
using ::ndn::Data;
using ::ndn::Signature;
using ::ndn::Block;
using ::ndn::FaceUri;
using ::ndn::make_unique;

using ::nfd::face::Face;

    
} // namespace val    
} // namespace ndn
} // namespace ns3


#endif // VAL_COMMON_HPP