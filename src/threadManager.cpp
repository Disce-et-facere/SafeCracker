#include "threadManager.h"

ThreadManager::ThreadManager() : crackedCount(0) {}

ThreadManager::~ThreadManager() {
    for (auto& thread : threads) {
        thread.join();
    }
}

void ThreadManager::joinThreads() {
    for (auto& thread : threads) {
        thread.join();
    }
}

void ThreadManager::startComparison(const std::vector<std::string>& hashes, const std::string& filePath) {
    hashSet.insert(hashes.begin(), hashes.end()); 
    outputFilePath = "../cracked.txt"; // temp file

    int numThreads = std::thread::hardware_concurrency();

    threads.reserve(numThreads);

    auto totalTasksProcessed = std::make_shared<std::atomic<int>>(0);
    auto noMoreTasks = std::make_shared<std::atomic<bool>>(false);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(&ThreadManager::comparisonThread, this, totalTasksProcessed, noMoreTasks);
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
            totalTasksProcessed->fetch_add(1);
            cv.notify_one();
        } else {
            std::cerr << "Error parsing line: " << line << std::endl;
        }
    }

    inputFile.close();

    noMoreTasks->store(true);

    cv.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << std::endl;
    std::cout << "All tasks processed!" << std::endl;
}


void ThreadManager::storeHit(const std::string& hash, const std::string& password) {

    if (hash.empty() || password.empty()) {
        std::cerr << "Warning: Skipping hit with empty hash or password." << std::endl;
        return;
    }

    std::string trimmedPassword = password;
    trimmedPassword.erase(0, trimmedPassword.find_first_not_of(" \t\r\n"));
    trimmedPassword.erase(trimmedPassword.find_last_not_of(" \t\r\n") + 1);

    std::ofstream outputFile(outputFilePath, std::ios_base::app);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening file " << outputFilePath << " for writing." << std::endl;
        return;
    }

    outputFile << hash << "," << trimmedPassword << std::endl;

    // Close the output file
    outputFile.close();

    ++crackedCount;
    printProgress();
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
    int hashCount = hashSet.size();
    std::cout << "\rProgress: " << crackedCount << " / " << hashCount;
}

void ThreadManager::comparisonThread(std::shared_ptr<std::atomic<int>> totalTasksProcessed, std::shared_ptr<std::atomic<bool>> noMoreTasks) {
    while (true) {
        std::pair<std::string, std::string> task;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return !taskQueue.empty() || noMoreTasks->load(); });
            if (taskQueue.empty() && noMoreTasks->load()) break;
            if (taskQueue.empty()) continue;
            task = std::move(taskQueue.front());
            taskQueue.pop();
        }

        const std::string& hash = task.first;
        const std::string& password = task.second;

        if (hashSet.count(hash) > 0) {
            storeHit(hash, password);
        } 

        printProgress();

        totalTasksProcessed->fetch_sub(1);
    }
}