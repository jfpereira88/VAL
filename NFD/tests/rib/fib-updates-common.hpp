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

#ifndef NFD_TESTS_RIB_FIB_UPDATES_COMMON_HPP
#define NFD_TESTS_RIB_FIB_UPDATES_COMMON_HPP

#include "rib/fib-updater.hpp"

#include "rib-test-common.hpp"
#include "tests/identity-management-fixture.hpp"

#include <ndn-cxx/util/dummy-client-face.hpp>

namespace nfd {
namespace rib {
namespace tests {

inline bool
compareNameFaceIdCostAction(const FibUpdate& lhs, const FibUpdate& rhs)
{
  if (lhs.name < rhs.name) {
    return true;
  }
  else if (lhs.name == rhs.name) {
    if (lhs.faceId < rhs.faceId) {
      return true;
    }
    else if (lhs.faceId == rhs.faceId) {
      if (lhs.cost < rhs.cost) {
        return true;
      }
      else if (lhs.cost == rhs.cost) {
        return lhs.action < rhs.action;
      }
    }
  }

  return false;
}

class FibUpdatesFixture : public nfd::tests::IdentityManagementFixture
{
public:
  FibUpdatesFixture()
    : face(g_io, m_keyChain)
    , controller(face, m_keyChain)
    , fibUpdater(rib, controller)
  {
  }

  void
  insertRoute(const Name& name, uint64_t faceId,
              std::underlying_type<ndn::nfd::RouteOrigin>::type origin,
              uint64_t cost, std::underlying_type<ndn::nfd::RouteFlags>::type flags)
  {
    Route route = createRoute(faceId, origin, cost, flags);

    RibUpdate update;
    update.setAction(RibUpdate::REGISTER)
          .setName(name)
          .setRoute(route);

    simulateSuccessfulResponse();
    rib.beginApplyUpdate(update, nullptr, nullptr);
  }

  void
  eraseRoute(const Name& name, uint64_t faceId,
             std::underlying_type<ndn::nfd::RouteOrigin>::type origin)
  {
    Route route = createRoute(faceId, origin, 0, 0);

    RibUpdate update;
    update.setAction(RibUpdate::UNREGISTER)
          .setName(name)
          .setRoute(route);

    simulateSuccessfulResponse();
    rib.beginApplyUpdate(update, nullptr, nullptr);
  }

  void
  destroyFace(uint64_t faceId)
  {
    simulateSuccessfulResponse();
    rib.beginRemoveFace(faceId);
  }

  const FibUpdater::FibUpdateList&
  getFibUpdates()
  {
    fibUpdates.clear();
    fibUpdates = fibUpdater.m_updatesForBatchFaceId;
    fibUpdates.insert(fibUpdates.end(), fibUpdater.m_updatesForNonBatchFaceId.begin(),
                                        fibUpdater.m_updatesForNonBatchFaceId.end());

    return fibUpdates;
  }

  FibUpdater::FibUpdateList
  getSortedFibUpdates()
  {
    FibUpdater::FibUpdateList updates = getFibUpdates();
    updates.sort(&compareNameFaceIdCostAction);
    return updates;
  }

  void
  clearFibUpdates()
  {
    fibUpdater.m_updatesForBatchFaceId.clear();
    fibUpdater.m_updatesForNonBatchFaceId.clear();
  }

private:
  void
  simulateSuccessfulResponse()
  {
    rib.mockFibResponse = [] (const RibUpdateBatch&) { return true; };
    rib.wantMockFibResponseOnce = true;
  }

public:
  ndn::util::DummyClientFace face;
  ndn::nfd::Controller controller;

  Rib rib;
  FibUpdater fibUpdater;
  FibUpdater::FibUpdateList fibUpdates;
};

} // namespace tests
} // namespace rib
} // namespace nfd

#endif // NFD_TESTS_RIB_FIB_UPDATES_COMMON_HPP
