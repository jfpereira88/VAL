/**
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