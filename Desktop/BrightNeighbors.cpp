/** @file
    @brief App that looks at each step of the patterns to find simultaneously
   bright beacons, records them all pairwise, and outputs them sorted by
   straight-line distance.

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Internal Includes
#include "Patterns.h"
#include "Positions.h"

// Library/third-party includes
#include <Eigen/Eigen>

// Standard includes
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <tuple> // for std::tie to make comparisons simpler.
#include <utility>
#include <vector>

static const int MAX_AUTO_RUNS = 7;

using BeaconList = std::vector<int>;
using Eigen::Vector3d;

inline double beaconSquaredDistance(Point3Vector const &locationVec, int a, int b) {
  return (Vector3d::Map(locationVec[a].data()) - Vector3d::Map(locationVec[b].data())).squaredNorm();
}

struct BeaconAdjacentBright {
  BeaconAdjacentBright(Point3Vector const &locationVec, int idA, int idB, int patternStepNum)
      : beaconA_(std::min(idA, idB)), beaconB_(std::max(idA, idB)), patternStep_(patternStepNum),
        squaredDistance_(beaconSquaredDistance(locationVec, beaconA_, beaconB_)) {}
  int beaconA() const { return beaconA_; }
  int beaconB() const { return beaconB_; }
  int patternStep() const { return patternStep_; }
  double squaredDistance() const { return squaredDistance_; }
  int oneBasedBeaconA() const { return beaconA_ + 1; }
  int oneBasedBeaconB() const { return beaconB_ + 1; }
  double distance() const { return std::sqrt(squaredDistance()); }
  double cost() const { return 1. / squaredDistance(); }

  friend bool operator<(BeaconAdjacentBright const &a, BeaconAdjacentBright const &b);

private:
  int beaconA_;
  int beaconB_;
  int patternStep_;
  double squaredDistance_;
};

inline bool operator<(BeaconAdjacentBright const &a, BeaconAdjacentBright const &b) {
  /// sort by distance, then by pattern step.
  return std::tie(a.squaredDistance_, a.patternStep_, a.beaconA_, a.beaconB_) <
         std::tie(b.squaredDistance_, b.patternStep_, b.beaconA_, b.beaconB_);
}

std::ostream &operator<<(std::ostream &os, BeaconAdjacentBright const &adj) {
  os << "(" << std::setw(2) << adj.oneBasedBeaconA();
  os << "--" << std::setw(2) << adj.oneBasedBeaconB();
  os << ") @ " << std::setw(2) << adj.patternStep();
  os << " [" << std::setw(8) << adj.distance() << " mm]";
  return os;
}

using AdjacentBrightnessList = std::vector<BeaconAdjacentBright>;

namespace detail {
class BrightnessTracking {
public:
  explicit BrightnessTracking(Point3Vector const &locationVec) : locationVec_(locationVec) {}

  BrightnessTracking &operator=(BrightnessTracking const &) = delete;

  void startPatternStep(int patternStep) { currentPatternStep_ = patternStep; }

  void recordBrightBeacon(int newBright) {
    for (auto &existingBright : brightBeacons_) {
      adjacentBrightness_.emplace_back(locationVec_, existingBright, newBright, currentPatternStep_);
      listSorted_ = false;
    }
    brightBeacons_.push_back(newBright);
  }

  void endPatternStep() { moveBrightBeaconsToList(); }

  /// Gets the list, lazily sorted if required, on demand
  AdjacentBrightnessList const &getSortedAdjacentBrightnessList() {
    if (!listSorted_) {
      sortAdjacentList();
    }
    return adjacentBrightness_;
  }

private:
  void moveBrightBeaconsToList() {
    brightBeaconsPerStep_.emplace_back(std::move(brightBeacons_));
    brightBeacons_.clear();
  }
  void sortAdjacentList() {
    std::sort(begin(adjacentBrightness_), end(adjacentBrightness_));
    listSorted_ = true;
  }
  Point3Vector const &locationVec_;
  std::vector<BeaconList> brightBeaconsPerStep_;
  AdjacentBrightnessList adjacentBrightness_;
  bool listSorted_ = false;
  BeaconList brightBeacons_;
  int currentPatternStep_;
};
} // namespace detail

/// Takes in parallel location and pattern vectors, as well as an optional list
/// of one-based beacon IDs to "mask" (in this case, mark as being always dim),
/// and returns the completed, sorted AdjacentBrightnessList.
AdjacentBrightnessList computeAdjacentBrightnessList(Point3Vector const &locationVec,
                                                     std::vector<std::string> patternVec,
                                                     std::vector<int> const &oneBasedBeaconIdsToMask = {}) {
  if (locationVec.size() != patternVec.size()) {
    throw std::length_error("Location vec and pattern vec were different "
                            "sizes, but must be the same!");
  }

  // Took the patternVec by value since we want a locally-writable copy.
  auto patternLength = static_cast<int>(patternVec.back().size());
  auto numLeds = static_cast<int>(patternVec.size());

  /// Replace the pattern in every "masked" LED with all dim for the purposes of
  /// this tool.
  auto maskString = std::string(PATTERN_LENGTH, DIM_CHAR);
  for (auto &maskId : oneBasedBeaconIdsToMask) {
    // going to do range checking here
    patternVec.at(maskId - 1) = maskString;
  }

  /// Set up helper object for the remainder of the computation.
  auto brightness = detail::BrightnessTracking{locationVec};

  /// For each pattern step...
  for (int patternStep = 0; patternStep < patternLength; ++patternStep) {

    brightness.startPatternStep(patternStep);

    /// go through each beacon...
    for (int ledNum = 0; ledNum < numLeds; ++ledNum) {

      /// checking to see if it's bright
      if (patternVec[ledNum][patternStep] == BRIGHT_CHAR) {
        /// and if so, recording it with the brightness tracker (which takes
        /// care of the pairwise comparison, etc)
        brightness.recordBrightBeacon(ledNum);
      }
    }
    brightness.endPatternStep();
  }
  return brightness.getSortedAdjacentBrightnessList();
}

struct BeaconCost {
  int id;
  std::size_t count;
  double totalCost;
  double avgCost() const { return totalCost / static_cast<double>(count); }
  int oneBasedId() const { return id + 1; }
};

using BeaconCostComparator = std::function<bool(BeaconCost const &lhs, BeaconCost const &rhs)>;

/// operator > on the total (distance-based) cost, then count, then I suppose
/// the ID just for comparison sake.
inline bool compareBeaconCostByTotalDistanceCost(BeaconCost const &lhs, BeaconCost const &rhs) {
  return std::tie(lhs.totalCost, lhs.count, lhs.id) > std::tie(rhs.totalCost, lhs.count, rhs.id);
}

/// operator > on the count, then total (distance-based) cost, then I suppose
/// the ID just for comparison sake.
inline bool compareBeaconCostByCount(BeaconCost const &lhs, BeaconCost const &rhs) {
  return std::tie(lhs.count, lhs.totalCost, lhs.id) > std::tie(rhs.count, rhs.totalCost, rhs.id);
}

/// @param comparator A greater-than comparator of some sort that will bring
/// your most-expensive (by your desired operational definition of that term) to
/// the top.
std::vector<BeaconCost> getMostExpensiveLeds(AdjacentBrightnessList const &adj,
                                             BeaconCostComparator comparator = &compareBeaconCostByTotalDistanceCost) {
  using IndivCosts = std::vector<double>;
  // Each beacon with a simultaneous bright will have a vector of the "cost" of
  // each simultaneous bright period.
  std::vector<IndivCosts> allCosts;
  auto recordCost = [&](int id, double cost) {
    if (!(id < allCosts.size())) {
      // we need to enlarge the vector
      allCosts.resize(id + 1);
    }
    allCosts.at(id).push_back(cost);
  };
  // Go through and record the costs of each beacon.
  for (auto &adjElt : adj) {
    auto cost = adjElt.cost();
    recordCost(adjElt.beaconA(), cost);
    recordCost(adjElt.beaconB(), cost);
  }

  std::vector<BeaconCost> ret;

  // Now, we need to somehow reduce the vector of costs to a single cost per
  // LED. Right now, just doing a mean.
  auto n = static_cast<int>(allCosts.size());
  for (int id = 0; id < n; ++id) {
    auto const &thisBeaconCosts = allCosts[id];
    if (thisBeaconCosts.empty()) {
      // no costs for this beacon
      continue;
    }
    auto count = thisBeaconCosts.size();
    auto totalCost = std::accumulate(begin(thisBeaconCosts), end(thisBeaconCosts), 0.);
    ret.push_back(BeaconCost{id, count, totalCost});
  }

  /// Finally, the sort: want operator > in some sense.
  std::sort(begin(ret), end(ret), comparator);
  return ret;
}

struct OverallCosts {
  double totalCost = 0.;
  int count = 0;
  double avgCostPerPair() const { return totalCost / static_cast<double>(count); }
};

OverallCosts computeCostOfFullList(AdjacentBrightnessList const &adj) {
  OverallCosts ret;
  ret.count = adj.size();
  auto accumHelper = [](double prev, BeaconAdjacentBright const &cur) { return prev + cur.cost(); };
  ret.totalCost = std::accumulate(begin(adj), end(adj), 0., accumHelper);
  return ret;
}

void printOutput(int runNumber, std::vector<int> const &maskList, AdjacentBrightnessList const &adj,
                 std::vector<BeaconCost> const &beaconCost, OverallCosts const &overall) {
  /// make and immediately call the lambda, so we can use a stringstream to
  /// create and effectively return a string while keeping scopes narrow.
  auto makeRunNumberId = [](int run) {
    std::ostringstream os;
    os << " (Run #" << run << ")";
    return os.str();
  };
  const std::string runNumberId = makeRunNumberId(runNumber);

  static const auto DIVIDING_LINE = "--------------------\n";
  std::cout << DIVIDING_LINE;
  std::cout << "Run Information" << runNumberId << "\n" << DIVIDING_LINE << std::endl;
  auto printMaskList = [&] {
    std::cout << "Masked LEDs:" << runNumberId << "\n";
    for (auto &mask : maskList) {
      std::cout << mask << std::endl;
    }
    std::cout << "\n";
  };
  printMaskList();

  std::cout << "Adjacent brightness list" << runNumberId << "\n" << DIVIDING_LINE;

  for (auto &adjElt : adj) {
    std::cout << adjElt << std::endl;
  }

  std::cout << "\n\n\n";

  std::cout << "Beacon expense list" << runNumberId << "\n" << DIVIDING_LINE;
  for (auto &beacon : beaconCost) {
    std::cout << "Beacon: " << beacon.oneBasedId() << " \tCount: " << beacon.count
              << " \tTotal Cost: " << beacon.totalCost << " \tAvg rt Cost: " << std::sqrt(beacon.avgCost())
              << std::endl;
  }

  std::cout << "\n\n\n";

  std::cout << "Overall expense" << runNumberId << "\n" << DIVIDING_LINE;
  std::cout << "Total number of pairs:\t" << overall.count << std::endl;
  std::cout << "Total cost (counting each pair once):\t" << overall.totalCost << std::endl;
  std::cout << "Average cost per pair:\t" << overall.avgCostPerPair() << std::endl;
  std::cout << "\n";

  printMaskList();
  std::cout << "\n\n End of output for run" << runNumberId << "\n" << DIVIDING_LINE << "\n";
}

void autoCreateMask(int maxRuns, BeaconCostComparator const &comparator) {
  std::vector<int> maskList;
  /// turn off up to 4 leds (running 5 passes)
  for (int i = 0; i < MAX_AUTO_RUNS; ++i) {
    auto adj =
        computeAdjacentBrightnessList(OsvrHdkLedLocations_SENSOR0, OsvrHdkLedIdentifier_SENSOR0_PATTERNS, maskList);
    auto beaconCosts = getMostExpensiveLeds(adj, comparator);
    auto overall = computeCostOfFullList(adj);
    /// Dump current output.
    printOutput(i, maskList, adj, beaconCosts, overall);

    /// Now, add the most expensive beacon to the mask list for next round.
    auto newMaskOneBased = beaconCosts.front().oneBasedId();
    std::cout << "Adding one-based beacon ID " << newMaskOneBased << " to the mask list for next round." << std::endl;
    maskList.push_back(newMaskOneBased);
  }
}

int main() {
  initPatterns();
  // autoCreateMask(MAX_AUTO_RUNS, &compareBeaconCostByCount);
  autoCreateMask(MAX_AUTO_RUNS, &compareBeaconCostByTotalDistanceCost);

  return 0;
}
