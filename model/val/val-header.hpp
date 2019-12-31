/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_VALHEADER_HPP
#define VAL_VALHEADER_HPP

#include <ndn-cxx/encoding/block.hpp>
#include "ndn-cxx/encoding/encoding-buffer.hpp"
#include "ndn-cxx/lp/tlv.hpp"

namespace ns3 {
namespace ndn {
namespace val {

using ::ndn::Block;

/**
 * \brief represents a ValHeader field
 */
class ValHeader
{
public:
  class Error : public ::ndn::tlv::Error
  {
  public:
    using ::ndn::tlv::Error::Error;
  };

  ValHeader();

  explicit
  ValHeader(const Block& block);

  /**
   * \brief prepend ValHeader to encoder
   */
  template<::ndn::encoding::Tag TAG>
  size_t
  wireEncode(::ndn::EncodingImpl<TAG>& encoder) const;

  /**
   * \brief encode ValHeader into wire format
   */
  const Block&
  wireEncode() const;

  /**
   * \brief get ValHeader from wire format
   */
  void
  wireDecode(const Block& wire);

  std::string
  getMsg() const;

private:
  mutable Block m_wire;
  std::string m_msg;
};

NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(ValHeader);

}
}
}
#endif