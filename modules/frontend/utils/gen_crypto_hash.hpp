#ifndef GEN_CRYPTO_HASH_H
#define GEN_CRYPTO_HASH_H

#include <boost/uuid/sha1.hpp>
#include <string>

using namespace std;

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

#endif
