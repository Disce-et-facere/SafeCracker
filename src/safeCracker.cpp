#include <iomanip>
#include <packageManager.h>
#include <threadManager.h>
#include <hashManager.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <regex>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <utility>
#include <limits>
#include <shlobj.h>

class SafeCracker {
public:
    void crackMD5Hashes(const std::string& sourceFilePath, const std::string& targetFilePath) {
        std::ifstream inputFile(sourceFilePath);
        std::string formatLine;
        bool isCorrectFormat = false;

        if(std::getline(inputFile, formatLine)) {
            isCorrectFormat = isPasswordFileFormatted(formatLine);
        }

        if(!isCorrectFormat){
            std::cout << "The Source File Does Not Have Correct Format.(hash,password)\n";
            return;
        }

        std::ifstream inputFile2(targetFilePath);
        std::string hashLine;
        bool containsMD5Hashes = false;

        while (std::getline(inputFile2, hashLine)) {
            if (isMD5Hash(hashLine)) {
                containsMD5Hashes = true;
                break;
            }
        }

        if (!containsMD5Hashes) {
            std::cout << "The target File Does Not Contain MD5 Hashes.\n";
            //return;
        } else {
            addHashes(targetFilePath);
            
            std::cout << "Lets Get Crackin..." << std::endl;
            threadManager.startComparison(hashes, sourceFilePath);
                
        }
    }

    void crackSHA256Hashes(const std::string& sourceFilePath, const std::string& targetFilePath) {
        std::ifstream inputFile(sourceFilePath);
        std::string formatLine;
        bool isCorrectFormat = false;

        if(std::getline(inputFile, formatLine)) {
            isCorrectFormat = isPasswordFileFormatted(formatLine);
        }


        if(!isCorrectFormat){
            std::cout << "The Source File Does Not Have Correct Format.(hash,password)\n";
            return;
        }
        
        std::ifstream inputFile2(targetFilePath);
        std::string line;
        bool containsSHA256Hashes = false;

        while (std::getline(inputFile2, line)) {
            if (isSHA256Hash(line)) {
                containsSHA256Hashes = true;
                break;
            }
        }

        if (!containsSHA256Hashes) {
            std::cout << "The target file does not contain SHA256 hashes.\n";
        } else {

            addHashes(targetFilePath);
            
            std::cout << "Lets Get Crackin..." << std::endl;
            threadManager.startComparison(hashes, sourceFilePath);
            
        }
    }

    void formatSoure(std::string source, std::string target, int algorithm, bool variations){
        addPasswords(source);
        format(target,algorithm, variations);
    }

private:
    ThreadManager threadManager;
    std::vector<std::string> passwords;
    std::vector<std::string> hashes;
    std::vector<std::pair<std::string, std::string>> hashPasswordList;

    
    void addPasswords(std::string source){
        std::ifstream inputFile(source);
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file " << source << std::endl;
            return;
        }

        std::string password;
        while (inputFile >> password) {
            passwords.push_back(password);
        }

        inputFile.close();
    }

    void addFormattedSource(std::string source){

       std::ifstream inputFile(source);
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file " << source << std::endl;
            return;
        }

        std::string line;
        while (std::getline(inputFile, line)) {
            std::istringstream iss(line);
            std::string hash, password;
            if (std::getline(iss, hash, ',') && std::getline(iss, password)) {
                hashPasswordList.push_back(std::make_pair(hash, password));
            }
        }

        inputFile.close();

    }

    void addHashes(std::string target){
        std::ifstream inputFile(target);
        if (!inputFile.is_open()) {
            std::cerr << "Error opening file " << target << std::endl;
            return;
        }

        std::string hash;
        while (inputFile >> hash) {
            hashes.push_back(hash);
        }

        inputFile.close();
    }

    void format(std::string target, int algorithm, bool variations) {
        int totalPasswords = passwords.size();
        int processedPasswords = 0;

        std::string filePath = target + "\\variatedSource.txt";

        std::ofstream outputFile(filePath);
        if (!outputFile.is_open()) {
            std::cerr << "Error: Unable to open output file." << std::endl;
            return;
        }

        for (const auto& password : passwords) {
            auto package = variator(password, algorithm, variations);

            for (const auto& pair : package) {
                outputFile << pair.first << "," << pair.second << std::endl;
            }

            processedPasswords++;
            if(processedPasswords % 500 == 0){
            std::cout << "\r[ " << processedPasswords << " / " << totalPasswords << " ]" << std::flush;  
            }
        }

        std::cout << std::endl;
        std::cout << "Formatting Done!" << std::endl;
        std::cout << "Source File stored at: " << filePath << std::endl;
    }

    bool isMD5Hash(const std::string& str) {
     
        std::regex md5Pattern("^[0-9a-fA-F]{32}$");
        return std::regex_match(str, md5Pattern);
    }

    bool isSHA256Hash(const std::string& str) {
       
        std::regex sha256Pattern("^[0-9a-fA-F]{64}$");
        return std::regex_match(str, sha256Pattern);
    }

    bool isPasswordFileFormatted(const std::string& str) {
        std::istringstream iss(str);
        std::string hash, password;

        if (std::getline(iss, hash, ',') && std::getline(iss, password)) {
            return !hash.empty() && !password.empty();
        }

        return false;
    }

};


int main() {
    int choice;
    std::string sourceFilePath, targetFilePath;
    std::string singleHash;
    SafeCracker safecracker;

    do {

        std::cout << "Menu:\n";
        std::cout << "1. Crack MD5 Hashes\n";
        std::cout << "2. Crack SHA256 Hashes\n";
        std::cout << "3. Create Formatted Password File (MD5)\n";
        std::cout << "4. Create Formatted Password File (SHA-256)\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch(choice) {
            case 1:
                {
                    std::cout << "Crack MD5 Hashes.\n";

                    OPENFILENAME sourceOfn;
                    char sourceSzFile[260] = { 0 };
                    ZeroMemory(&sourceOfn, sizeof(sourceOfn));
                    sourceOfn.lStructSize = sizeof(sourceOfn);
                    sourceOfn.lpstrFile = sourceSzFile;
                    sourceOfn.lpstrFile[0] = '\0';
                    sourceOfn.nMaxFile = sizeof(sourceSzFile);
                    sourceOfn.lpstrFilter = "Text Files\0*.txt\0";
                    sourceOfn.nFilterIndex = 1;
                    sourceOfn.lpstrFileTitle = NULL;
                    sourceOfn.nMaxFileTitle = 0;
                    sourceOfn.lpstrInitialDir = NULL;
                    sourceOfn.lpstrTitle = "Select Source";
                    sourceOfn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
                    if (GetOpenFileName(&sourceOfn) == TRUE) {
                        sourceFilePath = sourceOfn.lpstrFile;
                        std::cout << "Selected source file: " << sourceFilePath << std::endl;
                    } else {
                        std::cout << "No source file selected.\n";
                        break;
                    }

                    // Open file dialog for target file
                    OPENFILENAME targetOfn;
                    char targetSzFile[260] = { 0 };
                    ZeroMemory(&targetOfn, sizeof(targetOfn));
                    targetOfn.lStructSize = sizeof(targetOfn);
                    targetOfn.lpstrFile = targetSzFile;
                    targetOfn.lpstrFile[0] = '\0';
                    targetOfn.nMaxFile = sizeof(targetSzFile);
                    targetOfn.lpstrFilter = "Text Files\0*.txt\0";
                    targetOfn.nFilterIndex = 1;
                    targetOfn.lpstrFileTitle = NULL;
                    targetOfn.nMaxFileTitle = 0;
                    targetOfn.lpstrInitialDir = NULL;
                    targetOfn.lpstrTitle = "Select Target";
                    targetOfn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
                    if (GetOpenFileName(&targetOfn) == TRUE) {
                        targetFilePath = targetOfn.lpstrFile;
                        std::cout << "Selected target file: " << targetFilePath << std::endl;
                    } else {
                        std::cout << "No target file selected.\n";
                        break;
                    }

                    safecracker.crackMD5Hashes(sourceFilePath, targetFilePath);
                    break;
                }
            case 2:
                {
                    std::cout << "Crack SHA256 Hashes.\n";

                    OPENFILENAME sourceOfn;
                    char sourceSzFile[260] = { 0 };
                    ZeroMemory(&sourceOfn, sizeof(sourceOfn));
                    sourceOfn.lStructSize = sizeof(sourceOfn);
                    sourceOfn.lpstrFile = sourceSzFile;
                    sourceOfn.lpstrFile[0] = '\0';
                    sourceOfn.nMaxFile = sizeof(sourceSzFile);
                    sourceOfn.lpstrFilter = "Text Files\0*.txt\0";
                    sourceOfn.nFilterIndex = 1;
                    sourceOfn.lpstrFileTitle = NULL;
                    sourceOfn.nMaxFileTitle = 0;
                    sourceOfn.lpstrInitialDir = NULL;
                    sourceOfn.lpstrTitle = "Select Source";
                    sourceOfn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
                    if (GetOpenFileName(&sourceOfn) == TRUE) {
                        sourceFilePath = sourceOfn.lpstrFile;
                        std::cout << "Selected source file: " << sourceFilePath << std::endl;
                    } else {
                        std::cout << "No source file selected.\n";
                        break; 
                    }

                    OPENFILENAME targetOfn;
                    char targetSzFile[260] = { 0 };
                    ZeroMemory(&targetOfn, sizeof(targetOfn));
                    targetOfn.lStructSize = sizeof(targetOfn);
                    targetOfn.lpstrFile = targetSzFile;
                    targetOfn.lpstrFile[0] = '\0';
                    targetOfn.nMaxFile = sizeof(targetSzFile);
                    targetOfn.lpstrFilter = "Text Files\0*.txt\0";
                    targetOfn.nFilterIndex = 1;
                    targetOfn.lpstrFileTitle = NULL;
                    targetOfn.nMaxFileTitle = 0;
                    targetOfn.lpstrInitialDir = NULL;
                    targetOfn.lpstrTitle = "Select Target";
                    targetOfn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
                    if (GetOpenFileName(&targetOfn) == TRUE) {
                        targetFilePath = targetOfn.lpstrFile;
                        std::cout << "Selected target file: " << targetFilePath << std::endl;
                    } else {
                        std::cout << "No target file selected.\n";
                        break; 
                    }

                    safecracker.crackSHA256Hashes(sourceFilePath, targetFilePath);
                    break;
                }  
            case 3:
                {
                    std::cout << "(MD5)Format Password File.\n";
                    char createVariationsChoice;
                    bool variations;
                    std::cout << "Do You Want To Create Password Variations [y/n]" << std::endl;
                    std::cin >> createVariationsChoice;
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                    if (createVariationsChoice != 'y' && createVariationsChoice != 'Y') {
                        std::cout << "Variations Not Selected!\n";
                        variations = false;
                    }else {
                        std::cout << "Variations Selected!.\n";
                        variations = true;
                    }

                    OPENFILENAME sourceOfn;
                    char sourceSzFile[260] = { 0 };
                    ZeroMemory(&sourceOfn, sizeof(sourceOfn));
                    sourceOfn.lStructSize = sizeof(sourceOfn);
                    sourceOfn.lpstrFile = sourceSzFile;
                    sourceOfn.lpstrFile[0] = '\0';
                    sourceOfn.nMaxFile = sizeof(sourceSzFile);
                    sourceOfn.lpstrFilter = "Text Files\0*.txt\0";
                    sourceOfn.nFilterIndex = 1;
                    sourceOfn.lpstrFileTitle = NULL;
                    sourceOfn.nMaxFileTitle = 0;
                    sourceOfn.lpstrInitialDir = NULL;
                    sourceOfn.lpstrTitle = "Select Password File";
                    sourceOfn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
                    if (GetOpenFileName(&sourceOfn) == TRUE) {
                        sourceFilePath = sourceOfn.lpstrFile;
                        std::cout << "Selected source file: " << sourceFilePath << std::endl;
                    } else {
                        std::cout << "No source file selected.\n";
                        break; 
                    }

                    char targetFolderPath[MAX_PATH] = { 0 };

                    BROWSEINFO browseInfo;
                    ZeroMemory(&browseInfo, sizeof(browseInfo));
                    browseInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;
                    browseInfo.lpfn = NULL;
                    browseInfo.lpszTitle = "Select Where To Store Variated Passwords Package";

                    LPITEMIDLIST pItemIdList = SHBrowseForFolder(&browseInfo);
                    if (pItemIdList != NULL) {
                        SHGetPathFromIDList(pItemIdList, targetFolderPath);
                        std::cout << "Selected target folder: " << targetFolderPath << std::endl;
                        CoTaskMemFree(pItemIdList);
                    } else {
                        std::cout << "No target folder selected.\n";
                        break;
                    }
                    std::cout << "Formatting..." << std::endl;

                    safecracker.formatSoure(sourceFilePath, targetFolderPath, 5,variations);
                    break;
                }
            case 4:
                {
                    std::cout << "(SHA256)Create Variated Passwords And Hashes.\n";

                    OPENFILENAME sourceOfn;
                    char sourceSzFile[260] = { 0 };
                    ZeroMemory(&sourceOfn, sizeof(sourceOfn));
                    sourceOfn.lStructSize = sizeof(sourceOfn);
                    sourceOfn.lpstrFile = sourceSzFile;
                    sourceOfn.lpstrFile[0] = '\0';
                    sourceOfn.nMaxFile = sizeof(sourceSzFile);
                    sourceOfn.lpstrFilter = "Text Files\0*.txt\0";
                    sourceOfn.nFilterIndex = 1;
                    sourceOfn.lpstrFileTitle = NULL;
                    sourceOfn.nMaxFileTitle = 0;
                    sourceOfn.lpstrInitialDir = NULL;
                    sourceOfn.lpstrTitle = "Select Source";
                    sourceOfn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
                    if (GetOpenFileName(&sourceOfn) == TRUE) {
                        sourceFilePath = sourceOfn.lpstrFile;
                        std::cout << "Selected source file: " << sourceFilePath << std::endl;
                    } else {
                        std::cout << "No source file selected.\n";
                        break; 
                    }

                    OPENFILENAME targetOfn;
                    char targetSzFile[260] = { 0 };
                    ZeroMemory(&targetOfn, sizeof(targetOfn));
                    targetOfn.lStructSize = sizeof(targetOfn);
                    targetOfn.lpstrFile = targetSzFile;
                    targetOfn.lpstrFile[0] = '\0';
                    targetOfn.nMaxFile = sizeof(targetSzFile);
                    targetOfn.lpstrFilter = "Text Files\0*.txt\0";
                    targetOfn.nFilterIndex = 1;
                    targetOfn.lpstrFileTitle = NULL;
                    targetOfn.nMaxFileTitle = 0;
                    targetOfn.lpstrInitialDir = NULL;
                    targetOfn.lpstrTitle = "Select Where To Store Variated Passwords Package";
                    targetOfn.Flags = OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
                    if (GetOpenFileName(&targetOfn) == TRUE) {
                        targetFilePath = targetOfn.lpstrFile;
                        std::cout << "Selected target file: " << targetFilePath << std::endl;
                    } else {
                        std::cout << "No target file selected.\n";
                        break;
                    }
                }
            case 0:
                {
                std::cout << "Exiting program.\n";
                return 0;                
                }
            default:
                std::cout << "Invalid choice. Please try again.\n";
        }
    } while(choice != 0);

    return 0;
}