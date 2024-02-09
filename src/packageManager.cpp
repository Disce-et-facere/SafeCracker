#include <algorithm>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <hashManager.h>
#include <packageManager.h>

std::unordered_map<char, std::vector<std::string>> variationMap = {
    {'A', {"a","4", "@", "Д"}},
    {'B', {"8"}},
    {'C', {"c"}},
    {'D', {"d"}},
    {'E', {"3", "£", "€", "e"}},
    {'F', {"7","ƒ","f"}},
    {'G', {"9", "6"}},
    {'H', {"#", "4","h"}},
    {'I', {"1", "|", "!","i"}},
    {'J', {"j","7", "9"}},
    {'K', {"k"}},
    {'L', {"7"}},
    {'M', {"m"}},
    {'N', {"n"}},
    {'O', {"0","o"}},
    {'P', {"p"}},
    {'Q', {"9","q"}},
    {'R', {"r"}},
    {'S', {"5", "$", "§","s"}},
    {'T', {"7","t"}},
    {'U', {"u"}},
    {'V', {"v"}},
    {'W', {"w"}},
    {'X', {"x"}},
    {'Y', {"y"}},
    {'Z', {"5","z"}}
};

std::vector<std::pair<std::string, std::string>> variator(const std::string& password, int algorithm, bool variate) {
    std::vector<std::pair<std::string, std::string>> variations;

    auto hash = [&algorithm](const std::string& str) -> std::string {
        std::string hash;
        if (algorithm == 5) {
            hash = HashManager::md5Hash(str);
        } else if (algorithm == 256) {
            hash = HashManager::sha256Hash(str);
        }
        return hash;
    };

    std::string originalHash = hash(password);
    variations.push_back(std::make_pair(originalHash, password));

    if(variate){

        for (int year = 1960; year <= 2024; ++year) {
            std::string yearStr = std::to_string(year);
            std::string hashYear = hash(password + yearStr);
            variations.push_back(std::make_pair(hashYear, password + yearStr));
        }

        for (int year = 60; year <= 99; ++year) {
            std::string yearStr = std::to_string(year);
            std::string hashYear = hash(password + yearStr);
            variations.push_back(std::make_pair(hashYear, password + yearStr));
        }

        for (int year = 0; year <= 24; ++year) {
            std::string yearStr = (year < 10) ? "0" + std::to_string(year) : std::to_string(year);
            std::string hashYear = hash(password + yearStr);
            variations.push_back(std::make_pair(hashYear, password + yearStr));
        }

        
        if (std::all_of(password.begin(), password.end(), ::isalpha)) {
            std::vector<std::pair<std::string, std::string>> letterVariations = generateLetterVariations(password, hash);
            variations.insert(variations.end(), letterVariations.begin(), letterVariations.end());
        }
    }

    return variations;
}

std::vector<std::pair<std::string, std::string>> generateLetterVariations(const std::string& word, std::function<std::string(const std::string&)> hash) {
    std::vector<std::pair<std::string, std::string>> letterVariations;
    std::unordered_set<std::string> seenVariations;

    std::function<void(std::string, size_t)> generateVariations = [&](std::string currentWord, size_t index) {
        if (index >= word.size()) {
            std::string hashWord = hash(currentWord);
            if (seenVariations.find(currentWord) == seenVariations.end()) {
                letterVariations.push_back(std::make_pair(hashWord, currentWord));
                seenVariations.insert(currentWord); 
            }
            return;
        }

        char letter = word[index];
        if (!std::isalpha(letter)) {
            generateVariations(currentWord + letter, index + 1);
            return;
        }

        std::vector<std::string> variations = variationMap[toupper(letter)];
        for (const std::string& variation : variations) {
            generateVariations(currentWord + variation, index + 1);
        }

        generateVariations(currentWord + letter, index + 1);
    };

    generateVariations("", 0);

    return letterVariations;
}