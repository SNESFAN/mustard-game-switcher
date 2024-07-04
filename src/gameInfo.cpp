
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

GameInfoData loadGameInfo(string filePath)
{
    GameInfoData game;
    std::ifstream file(filePath);
    if (file.is_open())
    {
        std::string line;
        std::getline(file, line); // Read the first line to check the format
        if (line.find("/mnt/mmc/MUOS/info/core/") == 0) // New format detected
        {
            // Close the current file and open the new file path specified in the first line
            file.close();
            file.open(line);
            if (!file.is_open())
            {
                std::cerr << "Error opening new file path: " << line << std::endl;
                return game; // Return an inactive game info if the new file can't be opened
            }
        }
        else
        {
            // If it's the old format, reset the file stream to read from the beginning again
            file.seekg(0, std::ios::beg);
        }

        game.active = true;
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

GameInfoData writeGameInfo(string filePath, GameInfoData game)
{
    ofstream file(filePath);
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

vector<GameInfoData> loadGameListAtPath(string folderPath)
{
    vector<filesystem::directory_entry> entries;
    vector<GameInfoData> games;

    // Check if directory exists
    if (filesystem::exists(folderPath))
    {
        for (const auto &entry : filesystem::directory_iterator(folderPath))
        {
            if (entry.path().extension() == ".cfg")
            {
                entries.push_back(entry);
            }
        }

        std::sort(entries.begin(), entries.end(), [](const filesystem::directory_entry &a, const filesystem::directory_entry &b)
                  { return filesystem::last_write_time(a) > filesystem::last_write_time(b); });

        for (const auto &entry : entries)
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
    }
    return games;
}

GameVisualData loadGameVisualData(GameInfoData game, string folderPath)
{
    GameVisualData visualData;
    visualData.active = game.active;
    visualData.filePath = "";

    // Get the subdirectories in the folderPath
    vector<string> subdirectories;
    for (const auto &entry : filesystem::directory_iterator(folderPath))
    {
        if (entry.is_directory())
        {
            subdirectories.push_back(entry.path());
        }
    }

    // Search for the game.name with .state.auto.png extension in the subdirectories
    for (const auto &subdirectory : subdirectories)
    {
        string searchPath = subdirectory + "/" + game.name + ".state.auto.png";
        if (filesystem::exists(searchPath))
        {
            visualData.filePath = searchPath;
            break;
        }
    }

    return visualData;
}