#ifndef FONT_H
#define FONT_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

/**
 * Draws wrapped text on the screen using the specified font and color.
 *
 * @param text The text to be drawn.
 * @param font The font to be used for rendering the text.
 * @param screen The SDL renderer to draw the text on.
 * @param x The x-coordinate of the top-left corner of the text.
 * @param y The y-coordinate of the top-left corner of the text.
 * @param width The maximum width of the text before wrapping.
 * @param color The color of the text.
 */
void drawTextWrapped(const std::string& text, TTF_Font* font, SDL_Renderer* screen, int x, int y, int width, const SDL_Color& color);
/**
 * Draws text on the screen using the specified font, position, and color.
 *
 * @param text The text to be drawn.
 * @param font The font to be used for rendering the text.
 * @param screen The SDL renderer on which the text will be drawn.
 * @param x The x-coordinate of the top-left corner of the text.
 * @param y The y-coordinate of the top-left corner of the text.
 * @param color The color of the text.
 */
void drawText(const std::string& text, TTF_Font* font, SDL_Renderer* screen, int x, int y, const SDL_Color& color);
/**
 * Draws text centered on the screen.
 *
 * @param text The text to be drawn.
 * @param font The font to be used for rendering the text.
 * @param screen The SDL renderer to draw the text on.
 * @param x The x-coordinate of the center of the text.
 * @param y The y-coordinate of the center of the text.
 * @param width The width of the text box.
 * @param color The color of the text.
 */
void drawTextCentered(const std::string& text, TTF_Font* font, SDL_Renderer* screen, int x, int y, int width, const SDL_Color& color);

#endif