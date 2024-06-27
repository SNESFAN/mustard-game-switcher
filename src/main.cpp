#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/reboot.h>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <cstdlib>

#include "font.h"
#include "gameInfo.h"
#include "mrenderer.h"
#include "enum.h"
#include "helpers/strHelpers.h"
#include "helpers/mathHelpers.h"

using namespace std;
namespace fs = std::filesystem;

bool needExit = false;
bool needShutdown = false;
int swWidth = 640;
int swHeight = 480;
int hwWidth = 640;
int hwHeight = 480;

#ifdef DEBUG
bool debugMode = true;
string MUOS_HISTORY_DIR = "/mnt/muOSDump/mnt/mmc/MUOS/info/history";
string MUOS_FAVORITE_DIR = "/mnt/muOSDump/mnt/mmc/MUOS/info/favourite";
string MUOS_SAVE_DIR;
#else
bool debugMode = false;
string MUOS_HISTORY_DIR = "/mnt/mmc/MUOS/info/history";
string MUOS_FAVORITE_DIR = "/mnt/mmc/MUOS/info/favourite";
string MUOS_SAVE_DIR;
#endif

string MUOS_configFile = "/mnt/mmc/MUOS/retroarch/retroarch.cfg";
string MUOS_logFile = "log.txt";
string ROM_GO = "/tmp/rom_go";

SDL_Color defaultTextColor = {255, 255, 255, 255};
SDL_Color shadowTextColor = {0, 0, 0, 225};
SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *defaultFont = nullptr;
TTF_Font *mdFont = nullptr;
TTF_Font *lgFont = nullptr;
TTF_Font *titleFont = nullptr;
SDL_Joystick *joystick = nullptr;

vector<GameInfoData> recentGameList;
vector<GameInfoData> favoriteGameList;
vector<GameInfoData> currentGameList;

GameInfoData selectedGame;
GameVisualData selectedGameVisual;
Mustard::Renderer *mrenderer = nullptr;
int selectedGameIndex;

AppState appState = APPSTATE_RECENTVIEW;
bool isListView = false;
bool isPictureView = false;
string listViewTitle;
double camX;
double camY;

double approachCamX;
double approachCamY;

int dirXInput = 0;
int dirYInput = 0;

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

/**
 * @brief Initializes the SDL library and sets up the necessary components for the game switcher.
 * 
 * This function initializes SDL, connects the joystick, creates a window and renderer, and initializes fonts.
 * 
 * @note Make sure to call this function before using any other SDL-related functionality.
 */
void initSDL()
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

    // Connect joystick 0
    SDL_JoystickEventState(SDL_ENABLE);
    joystick = SDL_JoystickOpen(0);
    
    // Set the window size
    window = SDL_CreateWindow("SDL2 Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, hwWidth, hwHeight, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer)
    { 
        delete mrenderer;
        mrenderer = new Mustard::Renderer(renderer);
    }

    // Initialize Fonts
    TTF_Init();
    defaultFont = TTF_OpenFont("assets/font/jgs5.ttf", 24);
    mdFont = TTF_OpenFont("assets/font/jgs5.ttf", 28);
    lgFont = TTF_OpenFont("assets/font/jgs5.ttf", 38);
    titleFont = TTF_OpenFont("assets/font/jgs5.ttf", 45);
}


/**
 * @brief Clears the renderer and sets the draw color to black.
 * 
 * This function clears the renderer by filling it with black color.
 * It is typically called before rendering any new frame.
 */
void startRender()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

/**
 * Renders a black screen by setting the render draw color to black and clearing the renderer.
 */
void renderBlackScreen()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}



/**
 * Renders the game switcher interface.
 * This function displays the background, header, image, and text for the game switcher.
 * It also handles the rendering of the list view and footer.
 */
void renderGameSwitcher()
{
    string background = "assets/theme/bg.png";
    mrenderer->draw(background, 0, 0, swWidth, swHeight);

    if (isPictureView)
    {
        approachCamX = 0;
        approachCamY = 0;
        string header = "assets/theme/header.png";

        if (selectedGame.active)
        {
            drawTextCentered(selectedGame.coreName, defaultFont, renderer, 0 - camX * 0.5, 216 - camY, 640, {255, 255, 255, 80});
        }

        if (selectedGameVisual.active)
        {
            string path = selectedGameVisual.filePath;
            auto *textureData = mrenderer->getData(path);
            if (textureData && textureData->width > 32 && textureData->height > 32)
            {
                mrenderer->drawPreserveAspect(path, 320 - camX, 240 - camY, 640, 400, 0, 1);
            }
        }

        mrenderer->draw(header, 0, 0, 640, 40);
        if (selectedGame.active)
        {
            string prettyName = selectedGame.name;
            int maxLen = 48;
            if (prettyName.length() > maxLen)
            {
                prettyName = prettyName.substr(0, maxLen - 2) + "...";
            }
            drawTextCentered(prettyName, defaultFont, renderer, 2, 7, 640, shadowTextColor);
            drawTextCentered(prettyName, defaultFont, renderer, 0, 4, 640, defaultTextColor);
        }
    }
    else if (isListView)
    {
        approachCamX = 0;
        const int leftMargin = 32;
        const int topMargin = 24;
        const int titleMargin = 60;
        const int lineHeight = 42;
        auto calcY = [](int y, int offset = 0)
        {
            return titleMargin + topMargin + y * lineHeight + offset;
        };

        int selectedY = calcY(selectedGameIndex, 0);
        approachCamY = (selectedY > 150) ? (selectedY - 150) : 0;

        drawText(listViewTitle, lgFont, renderer, leftMargin, topMargin - camY, {200, 175, 25, 255});
        for (int i = 0; i < currentGameList.size(); i++)
        {
            string prettyName = currentGameList[i].name;
            int maxLen = 48;
            int textY = calcY(i) - camY;

            if (textY > -35 && textY < 480 + 35)
            {
                int textX = leftMargin;
                if (prettyName.length() > maxLen)
                {
                    prettyName = prettyName.substr(0, maxLen - 2) + "...";
                }
                string text = prettyName;
                if (i == selectedGameIndex)
                {
                    mrenderer->drawRect(0, textY - 5, 640, lineHeight, {255, 255, 255, 25});
                    drawText(text, defaultFont, renderer, textX, textY, {220, 190, 25, 255});
                }
                else
                {
                    drawText(text, defaultFont, renderer, textX, textY, defaultTextColor);
                }
            }
        }
    }

    string footer = "assets/theme/footer.png";
    mrenderer->draw(footer, 0, 440, 640, 40);
}

// Blit a color to the screen. Can be transparent
/**
 * Renders a filled rectangle with the specified color on the screen.
 *
 * @param color The color to render.
 */
void renderColor(SDL_Color color)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(renderer);
}



/**
 * Applies the rendered content to the screen.
 */
void applyRender()
{
    SDL_RenderPresent(renderer);
}



/**
 * Sets the application state and performs necessary operations based on the state.
 * 
 * @param state The new application state to set.
 */
void setAppState(AppState state)
{
    camY = -25;
    camX = 0;
    approachCamY = 0;
    approachCamX = 0;
    appState = state;
    selectedGameIndex = 0;
    selectedGame = {};
    selectedGameVisual = {};
    listViewTitle = "";
    isListView = false;
    isPictureView = false;
    currentGameList.clear();

    if (appState == APPSTATE_RECENTVIEW)
    {
        isPictureView = true;
        currentGameList = recentGameList;
    }
    else if (appState == APPSTATE_FAVORITELIST || appState == APPSTATE_RECENTLIST)
    {
        isListView = true;
        currentGameList = (appState == APPSTATE_FAVORITELIST) ? favoriteGameList : recentGameList;
        listViewTitle = (appState == APPSTATE_FAVORITELIST) ? "Favorites" : "Recent Games";
        std::sort(currentGameList.begin(), currentGameList.end(), [](const GameInfoData &a, const GameInfoData &b)
                  { return a.name < b.name; });
    }
}

/**
 * Changes the application state to the next state based on the given increment.
 *
 * @param i The increment value used to determine the next state.
 */
void nextAppState(int i)
{
    int numStates = 3;
    int nextState = (appState + i) % numStates;
    setAppState(static_cast<AppState>(nextState));
}

/**
 * @brief Updates the application state based on the current view mode.
 * 
 * This function is responsible for updating the application state based on the current view mode.
 * If the picture view mode is active, it increments the selected game index and adjusts the camera position.
 * If the list view mode is active, it decrements the selected game index and adjusts it based on the input direction.
 * The selected game is updated based on the current game list and the selected game index.
 * The selected game visual data is loaded if the selected game is active and the picture view mode is active.
 */
void updateAppState()
{
    if (isPictureView)
    {
        selectedGameIndex += dirXInput;
        camX -= dirXInput * 32;
    }
    else if (isListView)
    {
        selectedGameIndex -= dirYInput;

        int jumpSize = 4;
        selectedGameIndex += dirXInput * jumpSize;
        selectedGameIndex = std::clamp(selectedGameIndex, 0, static_cast<int>(currentGameList.size()) - 1);
    }

    selectedGame = (!currentGameList.empty()) ? currentGameList[(selectedGameIndex + currentGameList.size()) % currentGameList.size()] : GameInfoData{};
    selectedGameVisual = (selectedGame.active && isPictureView) ? loadGameVisualData(selectedGame, MUOS_SAVE_DIR) : GameVisualData{};
}

/**
 * @brief Starts the SDL phase of the game switcher.
 * 
 * This function initializes SDL, sets the application state to recent view,
 * and enters the SDL update loop. It handles various events such as button
 * presses and joystick input, updates the application state, and renders
 * the game switcher. The loop continues until a condition is met to start
 * the next phase.
 */
void startSDLPhase()
{
    initSDL();

    setAppState(APPSTATE_RECENTVIEW);

    startRender();
    renderGameSwitcher();
    applyRender();

    // Wait 100 ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    SDL_Event event;
    bool startNextPhase = false;
    double dasTimer = 0.0;
    double deltaTime = 0.0;
    double shutoffHoldTimer = 0.0;
    double sdlTime = 0.0;
    uint32_t lastTicks = SDL_GetTicks();

    // SDL Update loop
    while (!startNextPhase)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                needExit = true;
                startNextPhase = true;
                break;
            }

            if ((event.type == SDL_JOYBUTTONDOWN && event.jbutton.button == RGBUTTON_B) ||
                (event.type == SDL_JOYBUTTONDOWN && event.jbutton.button == RGBUTTON_SELECT) ||
                (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_B))
            {
                nextAppState(1);
            }
        }

        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
        // Exit if the user presses X
        if (SDL_JoystickGetButton(joystick, RGBUTTON_X) || keyboardState[SDL_SCANCODE_X])
        {
            needExit = true;
            startNextPhase = true;
        }

        // Select a game when A or SPACE is pressed
        if (SDL_JoystickGetButton(joystick, RGBUTTON_A) || keyboardState[SDL_SCANCODE_SPACE])
        {
            startNextPhase = true;
        }

        dirXInput = 0;
        dirYInput = 0;
        // Handle Directional input
        {
            int desiredXIndex = 0;
            int desiredYIndex = 0;
            if (SDL_JoystickGetHat(joystick, 0) == RGPAD_RIGHT || keyboardState[SDL_SCANCODE_RIGHT])
            {
                desiredXIndex = 1;
            }
            else if (SDL_JoystickGetHat(joystick, 0) == RGPAD_LEFT || keyboardState[SDL_SCANCODE_LEFT])
            {
                desiredXIndex = -1;
            }
            if (SDL_JoystickGetHat(joystick, 0) == RGPAD_DOWN || keyboardState[SDL_SCANCODE_DOWN])
            {
                desiredYIndex = -1;
            }
            else if (SDL_JoystickGetHat(joystick, 0) == RGPAD_UP || keyboardState[SDL_SCANCODE_UP])
            {
                desiredYIndex = 1;
            }

            if (desiredXIndex != 0 || desiredYIndex != 0)
            {
                // Tetris Auto-repeat
                if (dasTimer <= 0.0)
                {
                    dirXInput += desiredXIndex;
                    dirYInput += desiredYIndex;
                }
                else if (dasTimer > 0.2666)
                {
                    dirXInput += desiredXIndex;
                    dirYInput += desiredYIndex;
                    dasTimer -= 0.1;
                }
                dasTimer += deltaTime;
            }
            else
            {
                dasTimer = 0.0;
            }
        }

        updateAppState();

        // Shutdown if MENU or ESCAPE is pressed
        if (sdlTime > 0.1 && (SDL_JoystickGetButton(joystick, RGBUTTON_MENU) || keyboardState[SDL_SCANCODE_ESCAPE]))
        {
            // drawTextCentered("Hold to Power Off...", defaultFont, renderer, 0, 200, 640, defaultTextColor);
            if (shutoffHoldTimer > 0.34)
            {
                renderColor({0, 0, 0, 32});
            }
            else
            {

                renderColor({0, 0, 0, 5});
            }

            shutoffHoldTimer += deltaTime;
            if (SDL_JoystickGetButton(joystick, RGBUTTON_SELECT))
            {
                // If select is held, exit immediately
                shutoffHoldTimer += 1;
            }

            if (shutoffHoldTimer > 0.9)
            {
                needExit = true;
                needShutdown = true;
                startNextPhase = true;
            }
            applyRender();
            SDL_Delay(30);
        }
        else
        {
            shutoffHoldTimer = 0;
            // Render and wait for next frame
            startRender();
            renderGameSwitcher();
            applyRender();
        }

        // Update at 60 FPS if possible. Min Delay of 4
        SDL_Delay(4);
        while (SDL_GetTicks() - lastTicks < 16)
        {
            SDL_Delay(1);
        }
        uint32_t currentTicks = SDL_GetTicks();
        deltaTime = (currentTicks - lastTicks) / 1000.0;
        lastTicks = currentTicks;
        sdlTime += deltaTime;

        camY = lerp(camY, approachCamY, 0.1);
        camY = clamp(camY, approachCamY - 200, approachCamY + 200);

        camX = lerp(camX, approachCamX, 0.1);
        camX = clamp(camX, approachCamX - 200, approachCamX + 200);
        if (startNextPhase)
        {
            break;
        }
    }
}

/**
 * @brief Cleans up SDL resources and quits SDL.
 * 
 * This function destroys the renderer, window, and fonts used in the application,
 * and then quits the SDL library.
 */
void cleanupSDL()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(defaultFont);
    TTF_CloseFont(titleFont);
    TTF_Quit();
    SDL_Quit();
}

/**
 * @brief The main entry point of the program.
 *
 * This function is responsible for starting the game switcher and launching games.
 * It loads the recent game list and favorite game list, sets the current game list,
 * and initializes the SDL phase. It then checks if a shutdown or exit is needed,
 * and performs the necessary actions. If a game is selected, it writes the game info,
 * prepares for the game launch, and executes the command to launch the game.
 *
 * @param argc The number of command-line arguments.
 * @param argv An array of command-line arguments.
 * @return 0 if the program exits successfully, 1 otherwise.
 */
int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);

    cout << "Starting game switcher" << endl;

    for (int i = 0; i < 999; i++)
    {
        recentGameList = loadGameListAtPath(MUOS_HISTORY_DIR);
        favoriteGameList = loadGameListAtPath(MUOS_FAVORITE_DIR);
        currentGameList = recentGameList;
        MUOS_SAVE_DIR = pathvar().second;

        startSDLPhase();

        if (needShutdown)
        {
            printf("User has triggered a shutdown....\n");
            startRender();
            renderBlackScreen();
            applyRender();
            // Ensure everything is saved before shutting down
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            sync();
            std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            cleanupSDL();
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            if (!debugMode)
            {
                reboot(RB_POWER_OFF);
            }
            return 1;
        }
        else if (needExit)
        {
            printf("User has triggered an exit...\n");
            startRender();
            renderBlackScreen();
            applyRender();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            cleanupSDL();
            return 1;
        }

        if (selectedGame.active)
        {
            try
            {
                string launchPath = fs::current_path().string() + "/assets/sh/mylaunch.sh";
                printf("Launch Path: %s\n", launchPath.c_str());
                printf("Name: %s\n", selectedGame.name.c_str());

                printf("Writing Game Info\n");
                string historyPath = MUOS_HISTORY_DIR + "/" + selectedGame.name.substr(0, selectedGame.name.find_last_of(".")) + ".cfg";
                size_t zipPos = historyPath.find_last_of(".zip");
                if (zipPos != string::npos)
                {
                    historyPath = historyPath.substr(0, zipPos) + ".cfg";
                }

                if (!debugMode)
                {
                    writeGameInfo(historyPath, selectedGame);
                }
                printf("Finished writing Game Info\n\n");
                printf("Proceeding to game...\n");

                renderColor({0, 0, 0, 180});
                applyRender();

                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                cleanupSDL();
                sync();
                std::this_thread::sleep_for(std::chrono::milliseconds(25));

                string romPath = selectedGame.drive + selectedGame.folder + "/" + selectedGame.fileName;
                string cmd = "";

                // Check if /mnt/mmc/MUOS/.retroarch/retroarch.cfg exists
                if (fs::exists("/mnt/mmc/MUOS/.retroarch/retroarch.cfg"))
                {
                    // V10
                    cmd = "/mnt/mmc/MUOS/retroarch -c \"/mnt/mmc/MUOS/.retroarch/retroarch.cfg\" -L \"/mnt/mmc/MUOS/core/" + selectedGame.core + "\" \"" + romPath + "\"";
                }
                else
                {
                    // V11
                    cmd = "retroarch -c \"/mnt/mmc/MUOS/retroarch/retroarch.cfg\" -L \"/mnt/mmc/MUOS/core/" + selectedGame.core + "\" \"" + romPath + "\"";
                }
                printf("Executing Command: %s\n", cmd.c_str());
                // Execute the command when not in debug mode
                if (!debugMode)
                {
                    system(cmd.c_str());
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
                sync();
                std::this_thread::sleep_for(std::chrono::milliseconds(25));
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error when launching game. Exiting" << std::endl;
                std::cerr << e.what() << std::endl;

                cleanupSDL();
                return 1;
            }
        }
    }
    return 0;
}
