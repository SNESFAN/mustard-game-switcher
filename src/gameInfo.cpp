
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include "gameInfo.h"
#include "helpers/strHelpers.h"

using namespace std;

/**
 * Loads game information from a file.
 *
 * @param filePath The path to the file containing the game information.
 * @return The loaded game information.
 */
GameInfoData loadGameInfo(string filePath)
{
    GameInfoData game;

    std::ifstream file(filePath);
    if (file.is_open())
    {
        game.active = true;
        std::string line;
        int lineCount = 0;
        while (std::getline(file, line))
        {
            switch (lineCount)
            {
                case 0:
                    game.name = std::move(line);
                    break;
                case 1:
                    game.core = std::move(line);
                    break;
                case 2:
                    game.coreName = std::move(line);
                    break;
                case 3:
                    game.number = std::move(line);
                    break;
                case 4:
                    game.drive = std::move(line);
                    break;
                case 5:
                    game.folder = std::move(line);
                    break;
                case 6:
                    game.fileName = std::move(line);
                    break;
            }
            lineCount++;
        }
        file.close();
    }
    else
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
    }

    return game;
}

/**
 * Writes the game information to a file.
 * 
 * @param filePath The path of the file to write the game information to.
 * @param game The GameInfoData object containing the game information.
 * @return The GameInfoData object that was written to the file.
 */
GameInfoData writeGameInfo(string filePath, GameInfoData game)
{
    std::ofstream file(filePath);
    if (file.is_open())
    {
        file << game.name << std::endl;
        file << game.core << std::endl;
        file << game.coreName << std::endl;
        file << game.number << std::endl;
        file << game.drive << std::endl;
        file << game.folder << std::endl;
        file << game.fileName << std::endl;

        // Ensure data is synced to disk immediately
        file.flush();
        file.close();
    }
    else
    {
        std::cerr << "Error opening file: " << filePath << std::endl;
    }
    return game;
}

/**
 * Loads the game list from the specified folder path.
 * 
 * @param folderPath The path to the folder containing the game files.
 * @return A vector of GameInfoData objects representing the loaded game list.
 */
vector<GameInfoData> loadGameListAtPath(string folderPath)
{
    vector<GameInfoData> games;

    // Check if directory exists
    if (filesystem::exists(folderPath))
    {
        for (const auto &entry : filesystem::directory_iterator(folderPath))
        {
            if (entry.path().extension() == ".cfg")
            {
                GameInfoData gameInfo = loadGameInfo(entry.path());
                if (strEndsWith(gameInfo.core, "libretro.so"))
                {
                    games.push_back(gameInfo);
                }
            }
        }

        std::sort(games.begin(), games.end(), [](const GameInfoData &a, const GameInfoData &b)
                  { return a.core < b.core; });
    }
    return games;
}

/**
 * Loads the visual data for a game based on the provided game information and folder path.
 *
 * @param game The game information data.
 * @param folderPath The path to the folder containing the game visual data.
 * @return The loaded game visual data.
 */
GameVisualData loadGameVisualData(GameInfoData game, string folderPath)
{
    GameVisualData visualData;
    if (game.active && !game.coreName.empty() && !game.fileName.empty())
    {
        string normalizedCore = game.core;
        vector<string> coreSplit = strSplit(game.core, '_');
        if (coreSplit.size() > 1)
        {
            coreSplit.pop_back();
            normalizedCore = strJoin(coreSplit, "");
        }
        normalizedCore = strReplaceAll(normalizedCore, " ", "");
        normalizedCore = strReplaceAll(normalizedCore, "-", "");
        normalizedCore = strReplaceAll(normalizedCore, "_", "");
        normalizedCore = strToUpper(normalizedCore);

        vector<filesystem::directory_entry> subDirectories;
        for (const auto &entry : filesystem::directory_iterator(folderPath))
        {
            if (entry.is_directory())
            {
                subDirectories.push_back(entry);
            }
        }

        string partialMatchFolder;
        string exactMatchFolder;
        for (const auto &entry : subDirectories)
        {
            string foldername = entry.path().filename();
            string normalizedFolderName = strReplaceAll(strToUpper(foldername), " ", "");
            normalizedFolderName = strReplaceAll(normalizedFolderName, " ", "");
            normalizedFolderName = strReplaceAll(normalizedFolderName, "-", "");
            normalizedFolderName = strReplaceAll(normalizedFolderName, "_", "");

            if (normalizedCore == normalizedFolderName)
            {
                exactMatchFolder = entry.path().string();
                break;
            }
            else if (strStartsWith(normalizedFolderName, normalizedCore))
            {
                partialMatchFolder = entry.path().string();
            }
        }

        string subFolderPath = exactMatchFolder.empty() ? partialMatchFolder : exactMatchFolder;

        if (!subFolderPath.empty())
        {
            vector<filesystem::directory_entry> screenShots;

            if (filesystem::exists(subFolderPath))
            {
                for (const auto &entry : filesystem::directory_iterator(subFolderPath))
                {
                    std::string extension = entry.path().extension().string();
                    std::string fileName = entry.path().filename().string();
                    if (extension == ".png")
                    {
                        screenShots.push_back(entry);
                    }
                }
                std::sort(screenShots.begin(), screenShots.end(), [](const filesystem::directory_entry &a, const filesystem::directory_entry &b)
                          { return filesystem::last_write_time(a) > filesystem::last_write_time(b); });

                if (!screenShots.empty())
                {
                    visualData.active = true;
                    std::string gameFileName = game.fileName;
                    std::size_t dotIndex = gameFileName.find(".");
                    if (dotIndex != std::string::npos)
                    {
                        gameFileName = gameFileName.substr(0, dotIndex);
                    }
                    for (const auto &entry : screenShots)
                    {
                        std::string fileName = entry.path().filename().string();
                        std::size_t dotIndex = fileName.find(".");
                        if (dotIndex != std::string::npos)
                        {
                            fileName = fileName.substr(0, dotIndex);
                        }
                        if (fileName == gameFileName)
                        {
                            visualData.filePath = entry.path().string();
                            break;
                        }
                    }
                }
            }
        }
    }
    return visualData;
}
