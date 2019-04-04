/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "gen_crypto_hash.hpp"

#include <boost/format.hpp>
#include <boost/uuid/sha1.hpp>

#include "log.hpp"

void GenHash::setOutputSha1HashFile(std::string keysha1) {
  this->key_sha1_hash_file = keysha1;
}

std::string GenHash::getOutputSha1HashFile() {
  return this->key_sha1_hash_file;
}

void GenHash::setFilePath(std::string filepath) { this->filepath = filepath; }

std::string GenHash::getFilePath() { return this->filepath; }

int GenHash::generate_sha1_hash_for_file() {
  Map2Check::Log::Debug("Generating hash");
  unsigned int hash[5];
  std::stringstream ss;
  std::ifstream file(this->filepath.c_str(), std::ios::binary | std::ios::ate);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> buffer(size);
  if (file.read(buffer.data(), size)) {
    this->boost_sha1.process_bytes(buffer.data(), size);
    this->boost_sha1.get_digest(hash);
  } else {
    return -1;
  }
  for (std::size_t i = 0; i < sizeof(hash) / sizeof(hash[0]); ++i) {
    ss << std::hex << std::setfill('0') << std::setw(sizeof(int) * 2)
       << hash[i];
  }

  this->key_sha1_hash_file = ss.str();
  Map2Check::Log::Debug("Got hash " + key_sha1_hash_file);

  file.close();
  return 0;
}

void GenHash::print_sha1_hash_for_file() {
  std::cout << this->key_sha1_hash_file << std::endl;
}
