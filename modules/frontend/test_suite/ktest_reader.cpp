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

}  // namespace Map2Check
