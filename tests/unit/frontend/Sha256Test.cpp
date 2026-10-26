/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

// SHA-256 exists here for one reason: the Test-Comp test-suite metadata
// defines <programhash> as the SHA-256 of the program file, and Map2Check only
// had SHA-1 (Boost, in gen_crypto_hash.hpp). A wrong hash does not break a run
// -- it produces a suite that looks valid and misidentifies the program it was
// generated for -- so the implementation is pinned to published vectors rather
// than to a self-consistent round trip.

#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "../../../modules/frontend/utils/sha256.hpp"

// Vectors from FIPS 180-4 / RFC 6234.
TEST(Sha256Test, EmptyString) {
  EXPECT_EQ(Map2Check::sha256Hex(""),
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST(Sha256Test, Abc) {
  EXPECT_EQ(Map2Check::sha256Hex("abc"),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

// 448 bits: the length field no longer fits beside the padding in one block,
// so this forces the two-block tail path where naive implementations break.
TEST(Sha256Test, TwoBlockMessage) {
  EXPECT_EQ(Map2Check::sha256Hex(
                "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"),
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
}

TEST(Sha256Test, OneMillionA) {
  EXPECT_EQ(Map2Check::sha256Hex(std::string(1000000, 'a')),
            "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
}

// A file whose name and contents differ, so hashing the path instead of the
// contents cannot pass by coincidence.
TEST(Sha256Test, HashesFileContentsNotPath) {
  std::string path = "sha256_fixture.txt";
  {
    std::ofstream out(path, std::ios::binary);
    out << "abc";
  }
  EXPECT_EQ(Map2Check::sha256File(path),
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
  std::remove(path.c_str());
}

// Binary-safe: a NUL in the middle must not truncate the digest.
TEST(Sha256Test, HandlesEmbeddedNulBytes) {
  std::string withNul("a\0b", 3);
  ASSERT_EQ(withNul.size(), 3u);
  EXPECT_NE(Map2Check::sha256Hex(withNul), Map2Check::sha256Hex("a"));
  EXPECT_NE(Map2Check::sha256Hex(withNul), Map2Check::sha256Hex("ab"));
}

TEST(Sha256Test, MissingFileReturnsEmptyString) {
  EXPECT_EQ(Map2Check::sha256File("no_such_file_here.bin"), "");
}
