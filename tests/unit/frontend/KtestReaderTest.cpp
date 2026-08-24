/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// A .ktest is a binary format written by another project. Parsing it wrong
// does not crash: it yields plausible-looking numbers, which become plausible-
// looking <input> elements, which a validator scores as covering nothing. The
// failure is silent all the way down, so the parsing is pinned here against
// bytes this file builds by hand rather than against whatever KLEE happened to
// emit on the day.

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "../../../modules/frontend/test_suite/ktest_reader.hpp"

namespace fs = std::filesystem;

namespace {

// The format, from KLEE's ktest-tool: five-byte magic, then big-endian 32-bit
// counts. Version >= 2 carries two symbolic-argv fields before the objects.
class KtestBuilder {
 public:
  explicit KtestBuilder(uint32_t version = 3) : version_(version) {}

  KtestBuilder& object(const std::string& name,
                       const std::vector<uint8_t>& bytes) {
    objects_.push_back({name, bytes});
    return *this;
  }

  void writeTo(const fs::path& path, const char* magic = "KTEST") const {
    std::ofstream out(path, std::ios::binary);
    out.write(magic, 5);
    putBE(out, version_);
    putBE(out, 1);  // one argument, the bitcode path
    const std::string arg = "./program.bc";
    putBE(out, static_cast<uint32_t>(arg.size()));
    out.write(arg.data(), arg.size());
    if (version_ >= 2) {
      putBE(out, 0);  // symArgvs
      putBE(out, 0);  // symArgvLen
    }
    putBE(out, static_cast<uint32_t>(objects_.size()));
    for (const auto& object : objects_) {
      putBE(out, static_cast<uint32_t>(object.first.size()));
      out.write(object.first.data(), object.first.size());
      putBE(out, static_cast<uint32_t>(object.second.size()));
      if (!object.second.empty()) {
        out.write(reinterpret_cast<const char*>(object.second.data()),
                  object.second.size());
      }
    }
  }

 private:
  static void putBE(std::ofstream& out, uint32_t value) {
    const unsigned char raw[4] = {
        static_cast<unsigned char>((value >> 24) & 0xff),
        static_cast<unsigned char>((value >> 16) & 0xff),
        static_cast<unsigned char>((value >> 8) & 0xff),
        static_cast<unsigned char>(value & 0xff)};
    out.write(reinterpret_cast<const char*>(raw), 4);
  }

  uint32_t version_;
  std::vector<std::pair<std::string, std::vector<uint8_t>>> objects_;
};

std::vector<uint8_t> le32(int32_t value) {
  std::vector<uint8_t> bytes(4);
  std::memcpy(bytes.data(), &value, 4);
  return bytes;
}

std::vector<uint8_t> leDouble(double value) {
  std::vector<uint8_t> bytes(8);
  std::memcpy(bytes.data(), &value, 8);
  return bytes;
}

fs::path freshDir(const std::string& name) {
  fs::path d = fs::temp_directory_path() / ("m2c_ktest_" + name);
  fs::remove_all(d);
  fs::create_directories(d);
  return d;
}

Map2Check::KtestObject obj(const std::string& name,
                           const std::vector<uint8_t>& bytes) {
  return Map2Check::KtestObject{name, bytes};
}

}  // namespace

// --- parsing ----------------------------------------------------------------

TEST(ReadKtestFile, ReadsObjectsInOrder) {
  fs::path d = freshDir("order");
  KtestBuilder()
      .object("non_det_int", le32(42))
      .object("non_det_char", {'z'})
      .writeTo(d / "test000001.ktest");

  auto objects = Map2Check::readKtestFile((d / "test000001.ktest").string());
  ASSERT_EQ(objects.size(), 2u);
  EXPECT_EQ(objects[0].name, "non_det_int");
  EXPECT_EQ(objects[0].bytes.size(), 4u);
  EXPECT_EQ(objects[1].name, "non_det_char");
  EXPECT_EQ(objects[1].bytes.size(), 1u);
  fs::remove_all(d);
}

// KLEE has written both magics over its lifetime; refusing the older one would
// make the suite silently empty rather than loudly wrong.
TEST(ReadKtestFile, AcceptsTheLegacyBoutMagic) {
  fs::path d = freshDir("bout");
  KtestBuilder(1).object("non_det_int", le32(7)).writeTo(d / "a.ktest", "BOUT\n");
  auto objects = Map2Check::readKtestFile((d / "a.ktest").string());
  ASSERT_EQ(objects.size(), 1u);
  EXPECT_EQ(objects[0].name, "non_det_int");
  fs::remove_all(d);
}

// Version 1 has no symbolic-argv fields. Reading them anyway would consume
// eight bytes of the object table and produce garbage names.
TEST(ReadKtestFile, HandlesVersionOneWithoutSymArgvFields) {
  fs::path d = freshDir("v1");
  KtestBuilder(1).object("non_det_int", le32(5)).writeTo(d / "a.ktest");
  auto objects = Map2Check::readKtestFile((d / "a.ktest").string());
  ASSERT_EQ(objects.size(), 1u);
  EXPECT_EQ(objects[0].name, "non_det_int");
  fs::remove_all(d);
}

TEST(ReadKtestFile, RejectsAFileThatIsNotAKtest) {
  fs::path d = freshDir("garbage");
  { std::ofstream(d / "a.ktest") << "this is not a ktest file at all"; }
  EXPECT_TRUE(Map2Check::readKtestFile((d / "a.ktest").string()).empty());
  fs::remove_all(d);
}

TEST(ReadKtestFile, MissingFileYieldsNothing) {
  EXPECT_TRUE(Map2Check::readKtestFile("/nonexistent/a.ktest").empty());
}

// --- decoding ---------------------------------------------------------------

TEST(DecodeKtestObject, DecodesSignedIntegers) {
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("non_det_int", le32(42))), "42");
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("non_det_int", le32(-7))), "-7");
}

// The whole reason the object name had to be fixed. Read as unsigned, -1
// becomes 4294967295; read as signed, an unsigned 4294967295 becomes -1. Only
// the name says which.
TEST(DecodeKtestObject, SignednessComesFromTheName) {
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("non_det_int", le32(-1))), "-1");
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("non_det_unsigned", le32(-1))),
            "4294967295");
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("non_det_uint", le32(-1))),
            "4294967295");
}

// Narrow types must sign-extend from their own width, not from 32 bits.
TEST(DecodeKtestObject, SignExtendsFromTheObjectWidth) {
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("non_det_char", {0xff})), "-1");
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("non_det_uchar", {0xff})), "255");
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("non_det_short", {0xff, 0xff})),
            "-1");
}

TEST(DecodeKtestObject, DecodesDoubles) {
  const std::string text =
      Map2Check::decodeKtestObject(obj("non_det_double", leDouble(1.5)));
  EXPECT_EQ(text.rfind("1.5", 0), 0u) << "got " << text;
}

// An unknown name is a nondet type nobody has taught this decoder about yet --
// a reason to guess, not to drop the test case.
TEST(DecodeKtestObject, FallsBackToSignedForAnUnknownName) {
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("something_new", le32(-3))), "-3");
}

TEST(DecodeKtestObject, EmptyObjectDecodesToZero) {
  EXPECT_EQ(Map2Check::decodeKtestObject(obj("non_det_int", {})), "0");
}

// --- directory sweep --------------------------------------------------------

TEST(ReadKtestVectors, OneVectorPerKtestOrderedByName) {
  fs::path d = freshDir("sweep");
  KtestBuilder().object("non_det_int", le32(1)).writeTo(d / "test000002.ktest");
  KtestBuilder().object("non_det_int", le32(0)).writeTo(d / "test000001.ktest");
  KtestBuilder().object("non_det_int", le32(2)).writeTo(d / "test000003.ktest");

  auto vectors = Map2Check::readKtestVectors(d.string(), 100);
  ASSERT_EQ(vectors.size(), 3u);
  EXPECT_EQ(vectors[0][0], "0");
  EXPECT_EQ(vectors[1][0], "1");
  EXPECT_EQ(vectors[2][0], "2");
  fs::remove_all(d);
}

// klee-last holds .err, .kquery and statistics files alongside the vectors.
TEST(ReadKtestVectors, IgnoresEverythingThatIsNotAKtest) {
  fs::path d = freshDir("filter");
  KtestBuilder().object("non_det_int", le32(9)).writeTo(d / "test000001.ktest");
  { std::ofstream(d / "test000001.abort.err") << "error"; }
  { std::ofstream(d / "run.stats") << "stats"; }
  { std::ofstream(d / "assembly.ll") << "; ir"; }

  auto vectors = Map2Check::readKtestVectors(d.string(), 100);
  ASSERT_EQ(vectors.size(), 1u);
  EXPECT_EQ(vectors[0][0], "9");
  fs::remove_all(d);
}

TEST(ReadKtestVectors, StopsAtTheLimit) {
  fs::path d = freshDir("limit");
  for (int i = 0; i < 6; ++i) {
    char name[64];
    snprintf(name, sizeof(name), "test%06d.ktest", i);
    KtestBuilder().object("non_det_int", le32(i)).writeTo(d / name);
  }
  auto vectors = Map2Check::readKtestVectors(d.string(), 3);
  ASSERT_EQ(vectors.size(), 3u);
  EXPECT_EQ(vectors[2][0], "2");
  fs::remove_all(d);
}

// A path that read no input is not a test case; emitting one would put an
// empty <testcase> into the suite for every such path.
TEST(ReadKtestVectors, DropsVectorsWithNoObjects) {
  fs::path d = freshDir("empty");
  KtestBuilder().writeTo(d / "test000001.ktest");
  KtestBuilder().object("non_det_int", le32(4)).writeTo(d / "test000002.ktest");
  auto vectors = Map2Check::readKtestVectors(d.string(), 100);
  ASSERT_EQ(vectors.size(), 1u);
  EXPECT_EQ(vectors[0][0], "4");
  fs::remove_all(d);
}

TEST(ReadKtestVectors, MissingDirectoryYieldsNothing) {
  EXPECT_TRUE(Map2Check::readKtestVectors("/nonexistent/klee-last", 10).empty());
}
