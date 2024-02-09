#include <hashManager.h>
#include <iomanip>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <sstream>

std::string HashManager::md5Hash(const std::string &data) {
  unsigned char md[MD5_DIGEST_LENGTH];
  MD5(reinterpret_cast<const unsigned char *>(data.c_str()), data.length(), md);

  std::stringstream ss;
  for (const auto &byte : md) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }

  return ss.str();
}

std::string HashManager::sha256Hash(const std::string &data) {
  unsigned char sha256Digest[SHA256_DIGEST_LENGTH];
  SHA256(reinterpret_cast<const unsigned char *>(data.c_str()), data.length(),
         sha256Digest);

  std::stringstream ss;
  for (const auto &byte : sha256Digest) {
    ss << std::hex << std::setw(2) << std::setfill('0')
       << static_cast<int>(byte);
  }

  return ss.str();
}