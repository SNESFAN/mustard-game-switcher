#include "pathUtils.h"
#include <fstream>
#include <iostream>

std::string MUOS_logFile = "log.txt";
std::string MUOS_configFile = "/mnt/mmc/MUOS/retroarch/retroarch.cfg";

/**
 * Extracts the save file directory and save state directory paths from a config file.
 * 
 * @return A pair of strings representing the save file directory and save state directory paths.
 */
std::pair<std::string, std::string> pathvar() {
    std::ifstream configFile(MUOS_configFile);
    std::ofstream logFile(MUOS_logFile);

    std::string savefileDir, savestateDir;

    if (configFile.is_open() && logFile.is_open()) {
        std::string line;
        std::string targetWord1 = "savefile_directory";
        std::string targetWord2 = "savestate_directory";

        while (std::getline(configFile, line)) {
            if (line.find(targetWord1) == 0) {
                savefileDir = line.substr(line.find("\"") + 1, line.rfind("\"") - line.find("\"") - 1);
            } else if (line.find(targetWord2) == 0) {
                savestateDir = line.substr(line.find("\"") + 1, line.rfind("\"") - line.find("\"") - 1);
            }
        }

        if (!savefileDir.empty() || !savestateDir.empty()) {
            logFile << "Paths extracted and logged successfully." << std::endl;
            logFile << "Save File Path: " << savefileDir << std::endl;
            logFile << "Save State Path: " << savestateDir << std::endl;
        } else {
            std::cout << "No lines starting with \"" << targetWord1 << "\" and \"" << targetWord2 << "\" found in the config file." << std::endl;
        }
    } else {
        logFile << "Failed to open config file at static path:" << std::endl;
        logFile << MUOS_configFile << std::endl;
    }

    configFile.close();
    logFile.close();

    return std::make_pair(savefileDir, savestateDir);
}