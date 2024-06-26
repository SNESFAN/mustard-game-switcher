#ifndef TEXTURECACHE_H
#define TEXTURECACHE_H

#include <iostream>
#include <unordered_map>
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

namespace Mustard
{
    struct TextureData
    {
        std::string filename;
        SDL_Surface* surface;
        SDL_Texture* texture;
        int width;
        int height;
        double aspect;
    };

    class Renderer
    {
    public:
        std::unordered_map<std::string, TextureData> textureMap;
        SDL_Renderer* renderer;

        Renderer(SDL_Renderer* renderer) : renderer(renderer) {}

        /**
         * @brief Destructor for the Renderer class.
         * 
         * This destructor releases textures and cleans up resources if needed. It iterates over the textureMap
         * and destroys each texture using SDL_DestroyTexture(). Finally, it clears the textureMap.
         */
        ~Renderer()
        {
            // Release textures and clean up resources if needed
            for (auto& pair : textureMap)
            {
                SDL_DestroyTexture(pair.second.texture);
            }
            textureMap.clear();
        }

        /**
         * Retrieves the SDL_Texture associated with the specified filename.
         * 
         * @param filename The name of the file to retrieve the texture from.
         * @return The SDL_Texture associated with the filename, or nullptr if not found.
         */
        SDL_Texture* get(const std::string& filename)
        {
            TextureData* data = getData(filename);
            if (data)
            {
                return data->texture;
            }
            return nullptr;
        }

        /**
         * Retrieves the texture data for a given filename.
         * If the texture is already cached, it returns the cached data.
         * If the texture is not cached, it loads the surface and creates a texture from it,
         * caches the texture data, and returns the newly created texture data.
         *
         * @param filename The filename of the texture to retrieve.
         * @return A pointer to the TextureData object containing the texture information,
         *         or nullptr if the texture failed to load or create.
         */
        TextureData* getData(const std::string& filename)
        {
            // Check if texture is already cached
            if (textureMap.find(filename) != textureMap.end())
            {
                return &textureMap[filename];
            }

            // Load surface and texture
            SDL_Surface* surface = IMG_Load(filename.c_str());
            if (!surface)
            {
                std::cerr << "Failed to load surface: " << SDL_GetError() << std::endl;
                return nullptr;
            }

            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture)
            {
                std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
                SDL_FreeSurface(surface);
                return nullptr;
            }

            // Cache the texture
            TextureData data{ filename, surface, texture, surface->w, surface->h, static_cast<double>(surface->w) / static_cast<double>(surface->h) };
            textureMap[filename] = data;
            return &textureMap[filename];
        }

        /**
         * @brief Draws a texture on the screen at the specified position and size.
         * 
         * This function retrieves the texture associated with the given filename and draws it on the screen
         * at the specified position (x, y) with the specified width and height (w, h).
         * 
         * @param filename The filename of the texture to be drawn.
         * @param x The x-coordinate of the top-left corner of the destination rectangle.
         * @param y The y-coordinate of the top-left corner of the destination rectangle.
         * @param w The width of the destination rectangle.
         * @param h The height of the destination rectangle.
         */
        void draw(const std::string& filename, int x, int y, int w, int h)
        {
            SDL_Texture* texture = get(filename);
            if (texture)
            {
                draw(texture, x, y, w, h);
            }
        }

        /**
         * Draws a texture on the screen at the specified position and size.
         *
         * @param texture The SDL_Texture to be drawn.
         * @param x The x-coordinate of the top-left corner of the destination rectangle.
         * @param y The y-coordinate of the top-left corner of the destination rectangle.
         * @param w The width of the destination rectangle.
         * @param h The height of the destination rectangle.
         */
        void draw(SDL_Texture* texture, int x, int y, int w, int h)
        {
            SDL_Rect destRect = { x, y, w, h };
            SDL_RenderCopy(renderer, texture, nullptr, &destRect);
        }

        /**
         * @brief Draws a scaled texture on the screen.
         * 
         * This function takes a filename of an image, along with the center coordinates, width, height, rotation, and scale factors, and draws the corresponding texture on the screen.
         * If the texture is successfully loaded, it is drawn using the `drawScaled` function.
         * 
         * @param filename The filename of the image to be loaded as a texture.
         * @param centerX The x-coordinate of the center point of the texture.
         * @param centerY The y-coordinate of the center point of the texture.
         * @param w The width of the texture.
         * @param h The height of the texture.
         * @param rotation The rotation angle of the texture in degrees.
         * @param scale The scaling factor of the texture.
         */
        void drawScaled(const std::string& filename, double centerX, double centerY, double w, double h, double rotation, double scale)
        {
            SDL_Texture* texture = get(filename);
            if (texture)
            {
                drawScaled(texture, centerX, centerY, w, h, rotation, scale);
            }
        }

        /**
         * @brief Draws a scaled texture on the renderer.
         *
         * This function draws a texture on the renderer with the specified scaling, rotation, and position.
         *
         * @param texture The SDL_Texture to be drawn.
         * @param centerX The x-coordinate of the center of the destination rectangle.
         * @param centerY The y-coordinate of the center of the destination rectangle.
         * @param w The width of the destination rectangle.
         * @param h The height of the destination rectangle.
         * @param rotation The rotation angle in degrees.
         * @param scale The scaling factor.
         */
        void drawScaled(SDL_Texture* texture, double centerX, double centerY, double w, double h, double rotation, double scale)
        {
            SDL_Rect destRect = {
                static_cast<int>(centerX - ((w * 0.5) * scale)),
                static_cast<int>(centerY - ((h * 0.5) * scale)),
                static_cast<int>(w * scale),
                static_cast<int>(h * scale)
            };

            SDL_RenderCopyEx(renderer, texture, nullptr, &destRect, rotation, nullptr, SDL_FLIP_NONE);
        }

        /**
         * Draws an image with the specified filename while preserving its aspect ratio.
         * The image is centered at the specified coordinates (centerX, centerY) and scaled to fit within the specified width and height.
         * The image can also be rotated and scaled by the specified rotation and scale factors.
         *
         * @param filename The filename of the image to be drawn.
         * @param centerX The x-coordinate of the center point of the image.
         * @param centerY The y-coordinate of the center point of the image.
         * @param w The desired width of the image.
         * @param h The desired height of the image.
         * @param rotation The rotation angle of the image in degrees.
         * @param scale The scaling factor of the image.
         */
        void drawPreserveAspect(const std::string& filename, double centerX, double centerY, double w, double h, double rotation, double scale)
        {
            auto* data = getData(filename);
            if (data)
            {
                if (data->aspect < 1)
                {
                    drawScaled(filename, centerX, centerY, w, w / data->aspect, rotation, scale);
                }
                else
                {
                    drawScaled(filename, centerX, centerY, h * data->aspect, h, rotation, scale);
                }
            }
        }

        /**
         * Draws a filled rectangle on the renderer with the specified position, size, and color.
         *
         * @param x The x-coordinate of the top-left corner of the rectangle.
         * @param y The y-coordinate of the top-left corner of the rectangle.
         * @param w The width of the rectangle.
         * @param h The height of the rectangle.
         * @param color The color of the rectangle.
         */
        void drawRect(double x, double y, double w, double h, SDL_Color color)
        {
            if (color.a < 255)
            {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            }
            else
            {
                SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            }
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_Rect rect = { static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h) };
            SDL_RenderFillRect(renderer, &rect);
        }
    };
} // namespace Mustard

#endif // TEXTURECACHE_H