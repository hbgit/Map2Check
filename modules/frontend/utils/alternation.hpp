/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef MODULES_FRONTEND_UTILS_ALTERNATION_HPP_
#define MODULES_FRONTEND_UTILS_ALTERNATION_HPP_

#include <algorithm>
#include <cmath>

namespace Map2Check {

/** The two engines --alternate-engines takes turns with (tacas 3b spec). */
enum class Engine { Fuzzer, Klee };

/** The most a phase of `engine` may take in its `round`-th turn (1-based):
 * 0.1T for the fuzzer and 0.3T for KLEE, doubled every round, never more than
 * what is left. The doubling is what lets an engine the alternation did not
 * help come back with more time; an engine that stagnates earlier hands the
 * rest back at once. */
inline double alternationWindow(Engine engine, unsigned round, double budget,
                                double remaining) {
  const double base = (engine == Engine::Fuzzer ? 0.1 : 0.3) * budget;
  const double window = base * std::pow(2.0, round > 0 ? round - 1 : 0);
  return std::min(window, remaining);
}

/** How long an engine may go without new coverage before it is stagnant:
 * 5% of the budget, at least 10 s (15 s at the Test-Comp 300 s). Also the
 * least a phase must have left to be worth starting. */
inline unsigned stagnationSeconds(double budget) {
  return static_cast<unsigned>(std::max(10.0, 0.05 * budget));
}

/** Decides from successive coverage samples whether an engine stagnated: no
 * increase for `quietSeconds` since the last one. A sample of -1 means the
 * reading failed (KLEE's stats database busy or not written yet) and is no
 * evidence either way; the clock starts at the first real sample. */
class CoverageWatch {
 public:
  explicit CoverageWatch(double quietSeconds) : quiet(quietSeconds) {}

  bool stagnated(long long covered, double now) {
    if (covered < 0) return false;
    if (!seen || covered > last) {
      seen = true;
      last = covered;
      since = now;
      return false;
    }
    return now - since >= quiet;
  }

 private:
  double quiet;
  bool seen = false;
  long long last = 0;
  double since = 0;
};

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_ALTERNATION_HPP_
