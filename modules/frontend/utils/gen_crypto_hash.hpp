/**
 * Copyright (C) 2014 - 2019 Map2Check tool
 * This file is part of the Map2Check tool, and is made available under
 * the terms of the GNU General Public License version 3.
 **/

#ifndef MODULES_FRONTEND_UTILS_GEN_CRYPTO_HASH_HPP_
#define MODULES_FRONTEND_UTILS_GEN_CRYPTO_HASH_HPP_

#include <string>

#include <boost/uuid/sha1.hpp>

using std::string;

class GenHash {
 private:
  std::string filepath;

  std::string key_sha1_hash_file;

 public:
  boost::uuids::detail::sha1 boost_sha1;

  int generate_sha1_hash_for_file();

  void setFilePath(std::string filepath);

  std::string getFilePath();

  void setOutputSha1HashFile(std::string keysha1);

  std::string getOutputSha1HashFile();

  void print_sha1_hash_for_file();
};

#endif  // MODULES_FRONTEND_UTILS_GEN_CRYPTO_HASH_HPP_
