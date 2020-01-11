/**
 * jfp 2019
 * mieti - uminho
 */

#ifndef VAL_HEADER_HPP
#define VAL_HEADER_HPP

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
  static constexpr uint8_t MAXHOPS = 9;

  class Error : public ::ndn::tlv::Error
  {
  public:
    using ::ndn::tlv::Error::Error;
  };

  ValHeader();
  
  ValHeader(std::string SA, std::string DA, std::string phPos,
                        std::string RN, uint8_t hopC);

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

  // gets and sets
  std::string
  getSA() const;

  std::string
  getDA() const;

  std::string
  getPhPos() const;

  std::string
  getRN() const;

  uint8_t
  getHopC() const;
  
  void
  setSA(std::string SA);

  void
  setDA(std::string DA);

  void
  setPhPos(std::string phPos);

  void
  setRN(std::string RN);

  void
  setHopC(uint8_t hopC);


private:
  mutable Block m_wire;
  std::string m_SA;      // source area
  std::string m_DA;      // destination area
  std::string m_phPos;   // previous hop position
  std::string m_RN;      // root name
  uint8_t m_hopC;        // hop count decressing 
};

bool
operator==(const ValHeader& lhs, const ValHeader& rhs);


NDN_CXX_DECLARE_WIRE_ENCODE_INSTANTIATIONS(ValHeader);

}
}
}
#endif