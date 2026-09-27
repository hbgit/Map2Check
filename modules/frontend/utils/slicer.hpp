/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef MODULES_FRONTEND_UTILS_SLICER_HPP_
#define MODULES_FRONTEND_UTILS_SLICER_HPP_

#include <cstdint>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace Map2Check {

/** Every __VERIFIER_nondet_* function the slice must keep.
 *
 * The suite is generated on the slice, but TestCov runs it on the ORIGINAL
 * program. A nondet call the slicer drops -- its value does not reach the
 * criterion -- is still consumed by the original, so the vector shifts and
 * the suite stops covering (measured: ntdrivers/floppy.i.cil-1.c, 29 reads in
 * the program and 10 in the slice; FAILED, NOT_COVERED). Keeping these calls
 * as criteria keeps the consumption order.
 *
 * The first sixteen are what NonDetPass instruments; the rest are SV-COMP
 * names it does not model yet, kept so their order is not lost either. The
 * slicer accepts names the program does not use. */
inline const std::vector<std::string>& nondetFunctionNames() {
  static const std::vector<std::string> names = {
      "__VERIFIER_nondet_bool",     "__VERIFIER_nondet_char",
      "__VERIFIER_nondet_uchar",    "__VERIFIER_nondet_short",
      "__VERIFIER_nondet_ushort",   "__VERIFIER_nondet_int",
      "__VERIFIER_nondet_uint",     "__VERIFIER_nondet_unsigned",
      "__VERIFIER_nondet_long",     "__VERIFIER_nondet_ulong",
      "__VERIFIER_nondet_size_t",   "__VERIFIER_nondet_loff_t",
      "__VERIFIER_nondet_sector_t", "__VERIFIER_nondet_pointer",
      "__VERIFIER_nondet_pchar",    "__VERIFIER_nondet_double",
      "__VERIFIER_nondet_float",    "__VERIFIER_nondet_longlong",
      "__VERIFIER_nondet_ulonglong", "__VERIFIER_nondet__Bool",
      "__VERIFIER_nondet_u8",       "__VERIFIER_nondet_u16",
      "__VERIFIER_nondet_u32",      "__VERIFIER_nondet_charp"};
  return names;
}

/** The -c argument: the primary criteria, then every nondet function. */
inline std::string slicingCriteria(const std::vector<std::string>& primary) {
  std::ostringstream criteria;
  bool first = true;
  for (const std::string& name : primary) {
    criteria << (first ? "" : ",") << name;
    first = false;
  }
  for (const std::string& name : nondetFunctionNames()) {
    criteria << (first ? "" : ",") << name;
    first = false;
  }
  return criteria.str();
}

/** A weak definition of the criterion function, restoring the body the
 * slicer removes without displacing a real one. The signature must match
 * the program's declaration, or llvm-link rejects the module. */
inline std::string targetStubSource(const std::string& function) {
  if (function == "__VERIFIER_assert") {
    return "void __attribute__((weak)) __VERIFIER_assert(int cond) {}\n";
  }
  return "void __attribute__((weak)) " + function + "(void) {}\n";
}

struct SlicerCounts {
  unsigned globals = 0;
  unsigned functions = 0;
  unsigned blocks = 0;
  unsigned instructions = 0;
};

struct SlicerStatistics {
  bool found = false;  // both the "before" and the "after" line were read
  SlicerCounts before;
  SlicerCounts after;
};

/** Reads sbt-slicer's --statistics lines:
 *   Statistics before Globals/Functions/Blocks/Instr.: 37 97 2215 10764
 *   Statistics after Globals/Functions/Blocks/Instr.: 37 38 444 2989 */
inline SlicerStatistics parseSlicerStatistics(const std::string& slicerOutput) {
  static const std::regex line(
      R"(Statistics (before|after) Globals/Functions/Blocks/Instr\.:\s+)"
      R"((\d+)\s+(\d+)\s+(\d+)\s+(\d+))");
  SlicerStatistics stats;
  bool sawBefore = false;
  bool sawAfter = false;
  for (std::sregex_iterator it(slicerOutput.begin(), slicerOutput.end(), line),
       end;
       it != end; ++it) {
    const std::smatch& m = *it;
    SlicerCounts counts;
    counts.globals = static_cast<unsigned>(std::stoul(m[2]));
    counts.functions = static_cast<unsigned>(std::stoul(m[3]));
    counts.blocks = static_cast<unsigned>(std::stoul(m[4]));
    counts.instructions = static_cast<unsigned>(std::stoul(m[5]));
    if (m[1] == "before") {
      stats.before = counts;
      sawBefore = true;
    } else {
      stats.after = counts;
      sawAfter = true;
    }
  }
  stats.found = sawBefore && sawAfter;
  return stats;
}

/** The one log line a slice produces. Counts when the slicer reported them,
 * bytes always -- a slice narrows the question being answered, and this line
 * is the only visible sign of how much was dropped. */
inline std::string describeSlice(const std::string& criterion,
                                 const SlicerStatistics& stats,
                                 uintmax_t bytesBefore, uintmax_t bytesAfter) {
  std::ostringstream text;
  text << "Sliced with respect to " << criterion << ": ";
  if (stats.found) {
    text << stats.before.functions << "/" << stats.before.blocks << "/"
         << stats.before.instructions << " -> " << stats.after.functions << "/"
         << stats.after.blocks << "/" << stats.after.instructions
         << " functions/blocks/instructions (" << bytesBefore << " -> "
         << bytesAfter << " bytes of bitcode)";
  } else {
    text << bytesBefore << " -> " << bytesAfter << " bytes of bitcode";
  }
  return text.str();
}

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_SLICER_HPP_
