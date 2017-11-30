#include "gen_crypto_hash.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <boost/uuid/sha1.hpp>
#include <boost/format.hpp>


void GenHash::setOutputSha1HashFile(std::string keysha1) {
  this->key_sha1_hash_file = keysha1;
}

std::string GenHash::getOutputSha1HashFile() {
  return this->key_sha1_hash_file;
}

void GenHash::setFilePath(std::string filepath) {
  this->filepath = filepath;
}

std::string GenHash::getFilePath() {
  return this->filepath;
}


int GenHash::generate_sha1_hash_for_file() {    
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
  for (std::size_t i=0; i < sizeof(hash)/sizeof(hash[0]); ++i) {
    ss << std::hex << std::setfill('0') << std::setw(sizeof(int)*2)
       << hash[i];
  }

  this->key_sha1_hash_file = ss.str();
  return 0;
}

void GenHash::print_sha1_hash_for_file() {
  std::cout << this->key_sha1_hash_file << std::endl;
}
