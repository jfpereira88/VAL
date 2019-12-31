/**
 * jfp 2019
 * mieti - uminho
 */

#include "val-header.hpp"
#include "ndn-cxx/encoding/block-helpers.hpp"

namespace ns3 {
namespace ndn {
namespace val {


ValHeader::ValHeader()
    :m_msg("Hello from VAL")
{
}

ValHeader::ValHeader(const Block& block)
{
  wireDecode(block);
}

template<::ndn::encoding::Tag TAG>
size_t
ValHeader::wireEncode(::ndn::EncodingImpl<TAG>& encoder) const
{
  size_t length = 0;
  length += ::ndn::encoding::prependStringBlock(encoder, ::ndn::lp::tlv::ValHeaderMsg, m_msg);
  length += encoder.prependVarNumber(length);
  length += encoder.prependVarNumber(::ndn::lp::tlv::ValHeader);
  return length;
}

NDN_CXX_DEFINE_WIRE_ENCODE_INSTANTIATIONS(ValHeader);

const Block&
ValHeader::wireEncode() const
{

  if (m_wire.hasWire()) {
    return m_wire;
  }

  ::ndn::EncodingEstimator estimator;
  size_t estimatedSize = wireEncode(estimator);

  ::ndn::EncodingBuffer buffer(estimatedSize, 0);
  wireEncode(buffer);

  m_wire = buffer.block();

  return m_wire;
}

void
ValHeader::wireDecode(const Block& wire)
{
  if (wire.type() != ::ndn::lp::tlv::ValHeader) {
    BOOST_THROW_EXCEPTION(Error("expecting ValHeader block"));
  }

  m_wire = wire;
  m_wire.parse();

  Block::element_const_iterator it = m_wire.elements_begin();
  if (it != m_wire.elements_end() && it->type() == ::ndn::lp::tlv::ValHeaderMsg) {
    m_msg = ::ndn::encoding::readString(*it);
  }
  else {
    BOOST_THROW_EXCEPTION(Error("expecting ValHeaderMsg block"));
  }
}

std::string
ValHeader::getMsg() const {
    return m_msg;
}

} // namespace ns3
}
}
