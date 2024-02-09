#ifndef PACKAGEMANAGER_H
#define PACKAGEMANAGER_H

#include <string>
#include <vector>
#include <functional>

std::vector<std::pair<std::string, std::string>> variator(const std::string& password, int algorithm, bool variate);
std::vector<std::pair<std::string, std::string>> generateLetterVariations(const std::string& word, std::function<std::string(const std::string&)> hash);

#endif // PACKAGEMANAGER_H

