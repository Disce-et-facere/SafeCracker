#include "threadManager.h"

ThreadManager::ThreadManager() : crackedCount(0), unknownCount(0) {}

ThreadManager::~ThreadManager() {
    for (auto& thread : threads) {
        thread.join();
    }
    
    writeHitsToFile(outputFilePath);
}

void ThreadManager::startComparison(const std::vector<std::string>& hashes, const std::string& filePath) {
    hashSet.insert(hashes.begin(), hashes.end()); 
    outputFilePath = filePath + "/hits.txt"; 

    for (int i = 0; i < std::thread::hardware_concurrency(); ++i) {
        threads.emplace_back(&ThreadManager::comparisonThread, this);
    }

    std::ifstream inputFile(filePath);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening file " << filePath << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        std::string hash, password;
        if (std::getline(iss, hash, ',') && std::getline(iss, password)) {
            taskQueue.push(std::make_pair(hash, password));
        } else {
            std::cerr << "Error parsing line: " << line << std::endl;
        }
    }
    inputFile.close();
}

void ThreadManager::storeHit(const std::string& hash, const std::string& password) {
    hits.push_back(std::make_pair(hash, password));
    ++crackedCount;
}

void ThreadManager::writeHitsToFile(const std::string& filePath) {
    std::ofstream outputFile(filePath);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file " << filePath << " for writing." << std::endl;
        return;
    }

    for (const auto& hit : hits) {
        outputFile << hit.first << "," << hit.second << std::endl;
    }

    outputFile.close();
}

void ThreadManager::printProgress() {
    std::lock_guard<std::mutex> lock(mtx);
    std::cout << "Progress: " << crackedCount << " cracked / " << hashSet.size() << " total" << std::endl;
}

void ThreadManager::comparisonThread() {
    while (true) {
        std::pair<std::string, std::string> task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]() { return !taskQueue.empty(); });
            if (taskQueue.empty()) break;
            task = std::move(taskQueue.front());
            taskQueue.pop();
        }

        const std::string& hash = task.first;
        const std::string& password = task.second;

        if (hashSet.count(hash) > 0) {
            storeHit(hash, password);
        } else {
            std::lock_guard<std::mutex> lock(mtx);
            ++unknownCount;
        }

        printProgress();
    }
}
