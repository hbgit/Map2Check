/**
 * Copyright (C) 2014 - 2026 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 2.
 *
 * SPDX-License-Identifier: (GPL-2.0)
 **/

#include "sha256.hpp"

#include <array>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>

namespace Map2Check {
namespace {

/** First 32 bits of the fractional parts of the cube roots of the first 64
 * primes (FIPS 180-4, section 4.2.2). */
constexpr std::array<uint32_t, 64> kRoundConstants = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
    0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
    0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
    0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
    0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
    0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
    0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
    0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
    0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
    0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

inline uint32_t rotr(uint32_t x, unsigned n) {
  return (x >> n) | (x << (32u - n));
}

/** One 64-byte block, mixed into the running state (FIPS 180-4, 6.2.2). */
void compress(std::array<uint32_t, 8>* state, const uint8_t block[64]) {
  std::array<uint32_t, 64> w{};
  for (unsigned i = 0; i < 16; i++) {
    w[i] = (static_cast<uint32_t>(block[i * 4]) << 24) |
           (static_cast<uint32_t>(block[i * 4 + 1]) << 16) |
           (static_cast<uint32_t>(block[i * 4 + 2]) << 8) |
           static_cast<uint32_t>(block[i * 4 + 3]);
  }
  for (unsigned i = 16; i < 64; i++) {
    uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
    uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
    w[i] = w[i - 16] + s0 + w[i - 7] + s1;
  }

  std::array<uint32_t, 8> v = *state;
  for (unsigned i = 0; i < 64; i++) {
    uint32_t s1 = rotr(v[4], 6) ^ rotr(v[4], 11) ^ rotr(v[4], 25);
    uint32_t ch = (v[4] & v[5]) ^ (~v[4] & v[6]);
    uint32_t t1 = v[7] + s1 + ch + kRoundConstants[i] + w[i];
    uint32_t s0 = rotr(v[0], 2) ^ rotr(v[0], 13) ^ rotr(v[0], 22);
    uint32_t maj = (v[0] & v[1]) ^ (v[0] & v[2]) ^ (v[1] & v[2]);
    uint32_t t2 = s0 + maj;

    v[7] = v[6];
    v[6] = v[5];
    v[5] = v[4];
    v[4] = v[3] + t1;
    v[3] = v[2];
    v[2] = v[1];
    v[1] = v[0];
    v[0] = t1 + t2;
  }
  for (unsigned i = 0; i < 8; i++) (*state)[i] += v[i];
}

}  // namespace

std::string sha256Hex(const std::string& data) {
  /** First 32 bits of the fractional parts of the square roots of the first 8
   * primes (FIPS 180-4, section 5.3.3). */
  std::array<uint32_t, 8> state = {0x6a09e667u, 0xbb67ae85u, 0x3c6ef372u,
                                   0xa54ff53au, 0x510e527fu, 0x9b05688cu,
                                   0x1f83d9abu, 0x5be0cd19u};

  const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data.data());
  size_t length = data.size();
  size_t wholeBlocks = length / 64;
  for (size_t i = 0; i < wholeBlocks; i++) compress(&state, bytes + i * 64);

  // Tail: the leftover bytes, a 0x80 terminator, zero padding, then the
  // message length in bits as a big-endian 64-bit value. When the leftover
  // leaves no room for that length field this spills into a second block,
  // which is why the loop below is over tail.size() and not a single block.
  std::vector<uint8_t> tail(bytes + wholeBlocks * 64, bytes + length);
  tail.push_back(0x80);
  while (tail.size() % 64 != 56) tail.push_back(0x00);
  uint64_t bitLength = static_cast<uint64_t>(length) * 8u;
  for (int i = 7; i >= 0; i--) {
    tail.push_back(static_cast<uint8_t>((bitLength >> (i * 8)) & 0xffu));
  }
  for (size_t i = 0; i < tail.size(); i += 64)
    compress(&state, tail.data() + i);

  static const char kHex[] = "0123456789abcdef";
  std::string digest;
  digest.reserve(64);
  for (uint32_t word : state) {
    for (int i = 3; i >= 0; i--) {
      unsigned byte = (word >> (i * 8)) & 0xffu;
      digest += kHex[byte >> 4];
      digest += kHex[byte & 0xf];
    }
  }
  return digest;
}

std::string sha256File(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in.is_open()) return "";
  std::ostringstream buffer;
  buffer << in.rdbuf();
  return sha256Hex(buffer.str());
}

}  // namespace Map2Check
