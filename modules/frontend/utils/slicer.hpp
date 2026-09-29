/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#ifndef MODULES_FRONTEND_UTILS_SLICER_HPP_
#define MODULES_FRONTEND_UTILS_SLICER_HPP_

#include <algorithm>
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

/** Every __VERIFIER_nondet_* symbol in a module's textual IR (`opt -S`),
 * in order of first appearance. The fixed list above cannot know every name a
 * benchmark declares (int128, uint128, ...), and a name missing from the
 * criteria silently brings the shifted suite back. */
inline std::vector<std::string> nondetNamesInIR(const std::string& ir) {
  static const std::regex symbol(R"(@(__VERIFIER_nondet_[A-Za-z0-9_]+))");
  std::vector<std::string> names;
  for (std::sregex_iterator it(ir.begin(), ir.end(), symbol), end; it != end;
       ++it) {
    const std::string name = (*it)[1];
    if (std::find(names.begin(), names.end(), name) == names.end()) {
      names.push_back(name);
    }
  }
  return names;
}

/** Every map2check_* runtime symbol in a module's textual IR, in order of
 * first appearance. Used as the slicing criteria for the memory properties:
 * the property is decided by these calls, so none of them may be removed. */
inline std::vector<std::string> runtimeNamesInIR(const std::string& ir) {
  static const std::regex symbol(R"(@(map2check_[A-Za-z0-9_]+))");
  std::vector<std::string> names;
  for (std::sregex_iterator it(ir.begin(), ir.end(), symbol), end; it != end;
       ++it) {
    const std::string name = (*it)[1];
    if (std::find(names.begin(), names.end(), name) == names.end()) {
      names.push_back(name);
    }
  }
  return names;
}

/** Every function a module DECLARES without defining (`declare ... @f(`), in
 * order of first appearance, LLVM intrinsics excluded. In the modes that slice
 * the instrumented module these are criteria too: a memory error can happen
 * inside an external function (strcpy overflowing a stack buffer), and nothing
 * the runtime checks depends on such a call, so without it as a criterion the
 * slicer drops the call and the bug with it (CASTLE-787-2: a wrong TRUE). */
inline std::vector<std::string> externalNamesInIR(const std::string& ir) {
  static const std::regex declaration(
      R"((?:^|\n)declare [^\n]*?@([A-Za-z0-9_.$]+)\()");
  std::vector<std::string> names;
  for (std::sregex_iterator it(ir.begin(), ir.end(), declaration), end;
       it != end; ++it) {
    const std::string name = (*it)[1];
    // Intrinsics are not calls into code the slicer could keep -- except the
    // memory ones: clang lowers memcpy/memset/memmove (and struct copies) to
    // them, and an overflowing copy into a buffer nothing reads again feeds no
    // criterion, so it has to be one (the strcpy case again).
    if (name.rfind("llvm.", 0) == 0 && name.rfind("llvm.memcpy.", 0) != 0 &&
        name.rfind("llvm.memmove.", 0) != 0 &&
        name.rfind("llvm.memset.", 0) != 0) {
      continue;
    }
    if (std::find(names.begin(), names.end(), name) == names.end()) {
      names.push_back(name);
    }
  }
  return names;
}

/** The primary criteria for slicing the INSTRUMENTED module: every runtime
 * check, then every external function (see externalNamesInIR). Returns false
 * when the IR holds no runtime call at all -- a failed disassembly, an empty
 * file -- because a slice without the checks as criteria removes them all and
 * the property silently reads as holding. */
inline bool instrumentedSliceCriteria(const std::string& ir,
                                      std::vector<std::string>* criteria) {
  criteria->clear();
  for (const std::string& name : runtimeNamesInIR(ir)) {
    criteria->push_back(name);
  }
  if (criteria->empty()) return false;
  for (const std::string& name : externalNamesInIR(ir)) {
    if (std::find(criteria->begin(), criteria->end(), name) ==
        criteria->end()) {
      criteria->push_back(name);
    }
  }
  return true;
}

/** The -c argument: the primary criteria, then every nondet function -- the
 * fixed list plus `fromProgram` (nondetNamesInIR), each name once. */
inline std::string slicingCriteria(
    const std::vector<std::string>& primary,
    const std::vector<std::string>& fromProgram = {}) {
  std::vector<std::string> all(primary);
  auto add = [&all](const std::string& name) {
    if (std::find(all.begin(), all.end(), name) == all.end()) {
      all.push_back(name);
    }
  };
  for (const std::string& name : nondetFunctionNames()) add(name);
  for (const std::string& name : fromProgram) add(name);
  std::ostringstream criteria;
  for (size_t i = 0; i < all.size(); ++i) {
    criteria << (i == 0 ? "" : ",") << all[i];
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

/** Where the slice of each phase is kept for the next ones: <cwd>/<hash>.slice,
 * beside the scratch directory for the reason the seed store is (every phase
 * recreates the scratch). Holds <key>.bc for a slice, <key>.failed for a slicer
 * that failed or timed out -- which the next phase must not pay for again: on
 * eca-* the slicer spent 0.2T in phase 1 AND in phase 2, and the run was
 * killed past its budget with no suite (R15, 4 ERROR). */
inline std::string sliceCachePath(const std::string& cwd,
                                  const std::string& programHash) {
  return cwd + "/" + programHash + ".slice";
}

/** The cache key: FNV-1a (64-bit, hex) over the input bitcode's CONTENT and
 * every setting that shapes the slice, so a slice is reused only for the very
 * same question. Fields are separated by a byte no name contains. */
inline std::string sliceCacheKey(const std::string& inputContent,
                                 const std::string& criteriaLabel,
                                 const std::string& entry,
                                 const std::string& slicerFlags) {
  uint64_t hash = 14695981039346656037ull;
  auto mix = [&hash](const std::string& field) {
    for (unsigned char c : field) {
      hash ^= c;
      hash *= 1099511628211ull;
    }
    hash ^= 0xff;
    hash *= 1099511628211ull;
  };
  mix(inputContent);
  mix(criteriaLabel);
  mix(entry);
  mix(slicerFlags);
  static const char* digits = "0123456789abcdef";
  std::string key(16, '0');
  for (int i = 15; i >= 0; --i) {
    key[i] = digits[hash & 0xf];
    hash >>= 4;
  }
  return key;
}

/** The opt arguments for MAP2CHECK_SLICE_CLEANUP (an experiment knob): the dg
 * slicer leaves empty blocks and dead functions behind. "light" folds them
 * away; "o2" is the full pipeline, which may exploit undefined behaviour and
 * is measured, not trusted. Anything else means no cleanup. */
inline std::string sliceCleanupPasses(const std::string& knob) {
  if (knob == "light") return "-passes='function(simplifycfg,dce),globaldce'";
  if (knob == "o2") return "-O2";
  return "";
}

}  // namespace Map2Check

#endif  // MODULES_FRONTEND_UTILS_SLICER_HPP_
