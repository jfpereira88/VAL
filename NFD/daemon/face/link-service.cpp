/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014-2018,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "link-service.hpp"
#include "face.hpp"

namespace nfd {
namespace face {

NFD_LOG_INIT(LinkService);

LinkService::LinkService()
  : m_face(nullptr)
  , m_transport(nullptr)
{
}

LinkService::~LinkService()
{
}

void
LinkService::setFaceAndTransport(Face& face, Transport& transport)
{
  BOOST_ASSERT(m_face == nullptr);
  BOOST_ASSERT(m_transport == nullptr);

  m_face = &face;
  m_transport = &transport;
}

void
LinkService::sendInterest(const Interest& interest)
{
  BOOST_ASSERT(m_transport != nullptr);
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nOutInterests;

  doSendInterest(interest);

  afterSendInterest(interest);
}

void
LinkService::sendData(const Data& data)
{
  BOOST_ASSERT(m_transport != nullptr);
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nOutData;

  doSendData(data);

  afterSendData(data);
}

void
LinkService::sendNack(const ndn::lp::Nack& nack)
{
  BOOST_ASSERT(m_transport != nullptr);
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nOutNacks;

  doSendNack(nack);

  afterSendNack(nack);
}

void
LinkService::receiveInterest(const Interest& interest)
{
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nInInterests;

  afterReceiveInterest(interest);
}

void
LinkService::receiveData(const Data& data)
{
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nInData;

  afterReceiveData(data);
}

void
LinkService::receiveNack(const ndn::lp::Nack& nack)
{
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nInNacks;

  afterReceiveNack(nack);
}

void
LinkService::notifyDroppedInterest(const Interest& interest)
{
  ++this->nDroppedInterests;
  onDroppedInterest(interest);
}

std::ostream&
operator<<(std::ostream& os, const FaceLogHelper<LinkService>& flh)
{
  const Face* face = flh.obj.getFace();
  if (face == nullptr) {
    os << "[id=0,local=unknown,remote=unknown] ";
  }
  else {
    os << "[id=" << face->getId() << ",local=" << face->getLocalUri()
       << ",remote=" << face->getRemoteUri() << "] ";
  }
  return os;
}

/** 
 * ######################################
 *  ValNetFace support
 * ######################################
 */
void
LinkService::sendValPacket(const ::ns3::ndn::val::ValPacket& valPacket)
{
  doSendValPacket(valPacket);
}

void
LinkService::receiveValPacket(const ::ns3::ndn::val::ValPacket& valPacket)
{
  NFD_LOG_FACE_TRACE(__func__);
  afterReceiveValPkt(valPacket);
}

void
LinkService::doSendValPacket(const ::ns3::ndn::val::ValPacket& valPacket)
{
  NFD_LOG_FACE_TRACE("LinkService::doSendValPacket -> This should never happen!!!");
  BOOST_ASSERT(false);
}

/** 
 * ######################################
 *  GeoFace support
 * ######################################
 */
void
LinkService::sendInterestToForwarder(const Interest& interest)
{
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nInInterests;

  afterReceiveInterest(interest);
}

void
LinkService::sendDataToForwarder(const Data& data)
{
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nInData;

  afterReceiveData(data);
}

void
LinkService::sendInterestToVal(const Interest& interest)
{
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nOutInterests;

  doSendInterestToVal(interest);

  afterSendInterest(interest); 
}

void
LinkService::sendDataToVal(const Data& data, std::vector<uint32_t> *nonceList, bool isProducer)
{
  NFD_LOG_FACE_TRACE(__func__);

  ++this->nOutData;

  doSendDataToVal(data, nonceList, isProducer);

  afterSendData(data);
}

void
LinkService::doSendInterestToVal(const Interest& interest)
{
  NFD_LOG_FACE_TRACE("LinkService::doSendInterestToVal -> This should never happen!!!");
  BOOST_ASSERT(false);
}

void
LinkService::doSendDataToVal(const Data& data, std::vector<uint32_t> *nonceList, bool isProducer)
{
  NFD_LOG_FACE_TRACE("LinkService::doSendDataToVal -> This should never happen!!!");
  BOOST_ASSERT(false);
}

} // namespace face
} // namespace nfd
