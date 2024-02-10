#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <utility>
#include <queue>
#include <atomic>
#include <unordered_set>
#include <algorithm>
#include <memory>

class ThreadManager {
private:
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<std::thread> threads;
    std::unordered_set<std::string> hashSet;
    std::queue<std::pair<std::string, std::string>> taskQueue;
    std::vector<std::pair<std::string, std::string>> hits;
    std::atomic<int> crackedCount;
    std::atomic<int> totalChunks;
    std::atomic<int> processedChunks;
    std::string outputFilePath;

    void storeHit(const std::string& hash, const std::string& password);
    void printProgress();
    void comparisonThread(std::shared_ptr<std::atomic<int>> totalTasksProcessed, std::shared_ptr<std::atomic<bool>> noMoreTasks);

public:
    ThreadManager();
    ~ThreadManager();
    void joinThreads();
    void startComparison(const std::vector<std::string>& hashes, const std::string& filePath);
    void writeHitsToFile(const std::string& filePath);
};

#endif // THREADMANAGER_H
