#ifndef HASHENGINE_H
#define HASHENGINE_H

#include <string>

class HashManager {
public:
  static std::string md5Hash(const std::string &data);
  static std::string sha256Hash(const std::string &data);
};

#endif // HASHENGINE_H
