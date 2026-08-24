/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "ktest_reader.hpp"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <system_error>
#include <vector>

namespace Map2Check {

namespace {

// Layout, from KLEE's own ktest-tool: a five-byte magic, then big-endian
// 32-bit counts throughout. Version 2 and later carry two extra symbolic-argv
// fields between the arguments and the objects.
constexpr char kMagicKtest[] = "KTEST";
constexpr char kMagicBout[] = "BOUT\n";
constexpr size_t kMagicSize = 5;

// A sanity bound on any length read from the file. A truncated or foreign file
// yields huge counts, and resizing a vector to them is how a parser turns a
// bad input into an out-of-memory kill rather than a skipped file.
constexpr uint32_t kMaxReasonableLength = 1u << 24;  // 16 MiB

bool readBigEndian32(std::ifstream& in, uint32_t* out) {
  unsigned char raw[4];
  in.read(reinterpret_cast<char*>(raw), 4);
  if (in.gcount() != 4) return false;
  *out = (static_cast<uint32_t>(raw[0]) << 24) |
         (static_cast<uint32_t>(raw[1]) << 16) |
         (static_cast<uint32_t>(raw[2]) << 8) | static_cast<uint32_t>(raw[3]);
  return true;
}

void putBigEndian32(std::ofstream& out, uint32_t value) {
  const unsigned char raw[4] = {
      static_cast<unsigned char>((value >> 24) & 0xff),
      static_cast<unsigned char>((value >> 16) & 0xff),
      static_cast<unsigned char>((value >> 8) & 0xff),
      static_cast<unsigned char>(value & 0xff)};
  out.write(reinterpret_cast<const char*>(raw), 4);
}

/** The name and width KLEE uses for a NONDET_TYPE enumerator.
 *
 * Both halves have to agree with the runtime or a seed means nothing: the name
 * is what klee_make_symbolic was called with (NonDetGeneratorKlee.c), and the
 * width is what the fuzzer consumes per read (NonDetGeneratorLibFuzzy.c).
 * Enumerator values come from enum NONDET_TYPE in Map2CheckTypes.h. */
struct NonDetTypeInfo {
  const char* name;
  size_t width;
  bool isFloating;
  bool isUnsigned;
};

NonDetTypeInfo nonDetTypeInfo(int type) {
  switch (type) {
    case 0:  return {"non_det_int", sizeof(int), false, false};
    case 1:  return {"non_det_char", sizeof(char), false, false};
    case 2:  return {"non_det_pointer", sizeof(void*), false, true};
    case 3:  return {"non_det_ushort", sizeof(unsigned short), false, true};
    case 4:  return {"non_det_long", sizeof(int64_t), false, false};
    case 5:  return {"non_det_unsigned", sizeof(unsigned), false, true};
    case 6:  return {"non_det_ulong", sizeof(uint64_t), false, true};
    case 7:  return {"non_det_bool", sizeof(int), false, false};
    case 8:  return {"non_det_uchar", sizeof(unsigned char), false, true};
    case 10: return {"non_det_size_t", sizeof(size_t), false, true};
    case 13: return {"non_det_uint", sizeof(unsigned), false, true};
    case 14: return {"non_det_short", sizeof(short), false, false};
    case 15: return {"non_det_double", sizeof(double), true, false};
    default: return {nullptr, 0, false, false};
  }
}

/** Encodes a logged decimal value into the object's little-endian bytes. */
void encodeValue(const std::string& text, const NonDetTypeInfo& info,
                 std::vector<uint8_t>* out) {
  if (info.isFloating) {
    double value = 0.0;
    try {
      value = std::stod(text);
    } catch (const std::exception&) {
      value = 0.0;
    }
    std::memcpy(out->data(), &value,
                std::min(out->size(), sizeof(double)));
    return;
  }
  uint64_t raw = 0;
  try {
    // Parsed as signed first so a logged "-1" round-trips; the bit pattern is
    // the same either way, and the width below is what actually matters.
    raw = static_cast<uint64_t>(std::stoll(text));
  } catch (const std::exception&) {
    try {
      raw = std::stoull(text);
    } catch (const std::exception&) {
      raw = 0;
    }
  }
  for (size_t i = 0; i < out->size() && i < 8; ++i) {
    (*out)[i] = static_cast<uint8_t>((raw >> (8 * i)) & 0xff);
  }
}

bool skipBlock(std::ifstream& in) {
  uint32_t size = 0;
  if (!readBigEndian32(in, &size)) return false;
  if (size > kMaxReasonableLength) return false;
  in.seekg(size, std::ios::cur);
  return in.good();
}

/** Reads `size` bytes of little-endian integer into a 64-bit value.
 *
 * x86-64 is the only architecture this tool targets, and KLEE writes object
 * bytes in the target's own order. */
uint64_t leBytesToUnsigned(const std::vector<uint8_t>& bytes) {
  uint64_t value = 0;
  const size_t width = std::min<size_t>(bytes.size(), 8);
  for (size_t i = 0; i < width; ++i) {
    value |= static_cast<uint64_t>(bytes[i]) << (8 * i);
  }
  return value;
}

int64_t signExtend(uint64_t value, size_t byteWidth) {
  if (byteWidth == 0 || byteWidth >= 8) return static_cast<int64_t>(value);
  const uint64_t signBit = 1ull << (byteWidth * 8 - 1);
  if (value & signBit) {
    value |= ~((1ull << (byteWidth * 8)) - 1);
  }
  return static_cast<int64_t>(value);
}

bool isUnsignedName(const std::string& name) {
  static const char* kUnsigned[] = {"non_det_unsigned", "non_det_uint",
                                    "non_det_ulong",    "non_det_ushort",
                                    "non_det_uchar",    "non_det_size_t",
                                    "non_det_bool",     "non_det_pointer"};
  for (const char* candidate : kUnsigned) {
    if (name == candidate) return true;
  }
  return false;
}

}  // namespace

std::vector<KtestObject> readKtestFile(const std::string& path) {
  std::vector<KtestObject> objects;
  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) return objects;

  char magic[kMagicSize + 1] = {0};
  in.read(magic, kMagicSize);
  if (in.gcount() != static_cast<std::streamsize>(kMagicSize)) return objects;
  if (std::memcmp(magic, kMagicKtest, kMagicSize) != 0 &&
      std::memcmp(magic, kMagicBout, kMagicSize) != 0) {
    return objects;
  }

  uint32_t version = 0;
  if (!readBigEndian32(in, &version)) return objects;

  uint32_t numArgs = 0;
  if (!readBigEndian32(in, &numArgs)) return objects;
  if (numArgs > kMaxReasonableLength) return objects;
  for (uint32_t i = 0; i < numArgs; ++i) {
    if (!skipBlock(in)) return objects;
  }

  if (version >= 2) {
    uint32_t symArgvs = 0;
    uint32_t symArgvLen = 0;
    if (!readBigEndian32(in, &symArgvs)) return objects;
    if (!readBigEndian32(in, &symArgvLen)) return objects;
  }

  uint32_t numObjects = 0;
  if (!readBigEndian32(in, &numObjects)) return objects;
  if (numObjects > kMaxReasonableLength) return objects;

  for (uint32_t i = 0; i < numObjects; ++i) {
    uint32_t nameSize = 0;
    if (!readBigEndian32(in, &nameSize)) break;
    if (nameSize > kMaxReasonableLength) break;
    std::string name(nameSize, '\0');
    in.read(name.data(), nameSize);
    if (in.gcount() != static_cast<std::streamsize>(nameSize)) break;

    uint32_t dataSize = 0;
    if (!readBigEndian32(in, &dataSize)) break;
    if (dataSize > kMaxReasonableLength) break;
    std::vector<uint8_t> bytes(dataSize);
    if (dataSize > 0) {
      in.read(reinterpret_cast<char*>(bytes.data()), dataSize);
      if (in.gcount() != static_cast<std::streamsize>(dataSize)) break;
    }

    // A partial object is dropped, but the ones already read are kept: a
    // .ktest truncated by a kill still describes a usable prefix of the path,
    // and the same forgiving rule governs the CSV log.
    objects.push_back(KtestObject{name, std::move(bytes)});
  }

  return objects;
}

std::string decodeKtestObject(const KtestObject& object) {
  if (object.bytes.empty()) return "0";

  if (object.name == "non_det_double") {
    double value = 0.0;
    const size_t width = std::min<size_t>(object.bytes.size(), sizeof(double));
    std::memcpy(&value, object.bytes.data(), width);
    // %f-style, matching what the CSV log emits for a double, so a suite reads
    // the same whichever source produced it.
    std::string text = std::to_string(value);
    return text;
  }

  const uint64_t raw = leBytesToUnsigned(object.bytes);
  if (isUnsignedName(object.name)) {
    return std::to_string(raw);
  }
  // Signed by default, including for an unrecognised name. Getting the
  // signedness wrong turns -1 into 4294967295, which a validator rejects as
  // out of range far more visibly than it would a wrong-but-plausible value.
  return std::to_string(signExtend(raw, object.bytes.size()));
}

std::vector<std::string> readViolatingKtest(const std::string& kleeOutDir) {
  std::error_code error;
  if (!std::filesystem::is_directory(kleeOutDir, error)) return {};

  // KLEE names the report for a failing path testNNNNNN.<kind>.err beside
  // testNNNNNN.ktest -- .abort.err for an abort, .ptr.err for a bad
  // dereference, and so on. The stem up to the first dot is the path number,
  // which is the whole link between the two files.
  //
  // Only .abort.err. KLEE writes .ptr.err, .free.err, .div.err and others for
  // failures that are not this tool's target, and taking any of them would
  // return the vector of an unrelated path. Even .abort.err is not proof on
  // its own -- the sv-benchmarks assumption idiom aborts too -- which is why
  // the caller gates this on the runtime having recorded a violation.
  std::vector<std::string> candidates;
  const std::string kAbortSuffix = ".abort.err";
  for (const auto& entry :
       std::filesystem::directory_iterator(kleeOutDir, error)) {
    const std::string name = entry.path().filename().string();
    if (name.size() <= kAbortSuffix.size()) continue;
    if (name.compare(name.size() - kAbortSuffix.size(), kAbortSuffix.size(),
                     kAbortSuffix) != 0) {
      continue;
    }
    candidates.push_back(name.substr(0, name.size() - kAbortSuffix.size()));
  }
  // Sorted so that a run with several failing paths always yields the same
  // one. Arbitrary, but arbitrary-and-stable beats arbitrary-and-not: a suite
  // that changes between identical runs cannot be diffed.
  std::sort(candidates.begin(), candidates.end());

  for (const std::string& stem : candidates) {
    std::vector<KtestObject> objects =
        readKtestFile((std::filesystem::path(kleeOutDir) / (stem + ".ktest"))
                          .string());
    if (objects.empty()) continue;
    std::vector<std::string> inputs;
    inputs.reserve(objects.size());
    for (const KtestObject& object : objects) {
      inputs.push_back(decodeKtestObject(object));
    }
    return inputs;
  }
  return {};
}

std::vector<std::vector<std::string>> readKtestVectors(
    const std::string& kleeOutDir, size_t limit) {
  std::vector<std::vector<std::string>> vectors;
  std::error_code error;
  if (!std::filesystem::is_directory(kleeOutDir, error)) return vectors;

  // Collected and sorted rather than taken in directory order: readdir order
  // is filesystem-dependent, and KLEE's testNNNNNN numbering is the only
  // stable ordering available. A suite numbered differently on each machine
  // cannot be diffed or reproduced.
  std::vector<std::string> paths;
  for (const auto& entry :
       std::filesystem::directory_iterator(kleeOutDir, error)) {
    if (entry.path().extension() != ".ktest") continue;
    paths.push_back(entry.path().string());
  }
  std::sort(paths.begin(), paths.end());

  for (const std::string& path : paths) {
    if (vectors.size() >= limit) break;
    std::vector<KtestObject> objects = readKtestFile(path);
    if (objects.empty()) continue;

    std::vector<std::string> inputs;
    inputs.reserve(objects.size());
    for (const KtestObject& object : objects) {
      inputs.push_back(decodeKtestObject(object));
    }
    vectors.push_back(std::move(inputs));
  }
  return vectors;
}

std::vector<uint8_t> ktestToFuzzerBytes(
    const std::vector<KtestObject>& objects) {
  std::vector<uint8_t> bytes;
  for (const KtestObject& object : objects) {
    bytes.insert(bytes.end(), object.bytes.begin(), object.bytes.end());
  }
  return bytes;
}

bool writeKtestFile(const std::string& path,
                    const std::vector<KtestObject>& objects) {
  std::ofstream out(path, std::ios::binary);
  if (!out.is_open()) return false;

  out.write(kMagicKtest, kMagicSize);
  putBigEndian32(out, 3);  // version
  putBigEndian32(out, 1);  // one argument, mirroring what KLEE records
  const std::string argument = "map2check";
  putBigEndian32(out, static_cast<uint32_t>(argument.size()));
  out.write(argument.data(), static_cast<std::streamsize>(argument.size()));
  putBigEndian32(out, 0);  // symArgvs
  putBigEndian32(out, 0);  // symArgvLen
  putBigEndian32(out, static_cast<uint32_t>(objects.size()));
  for (const KtestObject& object : objects) {
    putBigEndian32(out, static_cast<uint32_t>(object.name.size()));
    out.write(object.name.data(),
              static_cast<std::streamsize>(object.name.size()));
    putBigEndian32(out, static_cast<uint32_t>(object.bytes.size()));
    if (!object.bytes.empty()) {
      out.write(reinterpret_cast<const char*>(object.bytes.data()),
                static_cast<std::streamsize>(object.bytes.size()));
    }
  }
  return out.good();
}

std::vector<KtestObject> readNonDetLogAsObjects(const std::string& csvPath) {
  std::vector<KtestObject> objects;
  std::ifstream in(csvPath);
  if (!in.is_open()) return objects;

  std::string line;
  while (std::getline(in, line)) {
    if (line.empty()) continue;
    std::vector<std::string> fields;
    std::string field;
    std::istringstream row(line);
    while (std::getline(row, field, ';')) fields.push_back(field);
    if (fields.size() < 7) continue;

    const std::string& value = fields[5];
    int type = 0;
    try {
      type = std::stoi(fields[6]);
    } catch (const std::exception&) {
      continue;
    }
    const NonDetTypeInfo info = nonDetTypeInfo(type);
    if (info.name == nullptr) continue;

    KtestObject object;
    object.name = info.name;
    object.bytes.assign(info.width, 0);
    encodeValue(value, info, &object.bytes);
    objects.push_back(std::move(object));
  }
  return objects;
}

}  // namespace Map2Check
