#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cstring>

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
void drawText(const std::string& text, TTF_Font* font, SDL_Renderer* screen, int x, int y, const SDL_Color& color)
{
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(screen, textSurface);
	SDL_Rect dstRect = { x, y, textSurface->w, textSurface->h };
	SDL_RenderCopy(screen, texture, nullptr, &dstRect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(texture);
}

/**
 * @brief Draws wrapped text on the screen using the specified font and color.
 * 
 * This function takes a string of text, a font, a renderer, and other parameters to draw the text on the screen.
 * The text will be wrapped within the specified width, and the color of the text can be customized.
 * 
 * @param text The text to be drawn on the screen.
 * @param font The font to be used for rendering the text.
 * @param screen The renderer used to draw the text.
 * @param x The x-coordinate of the top-left corner of the text.
 * @param y The y-coordinate of the top-left corner of the text.
 * @param width The maximum width of the wrapped text.
 * @param color The color of the text.
 */
void drawTextWrapped(const std::string& text, TTF_Font* font, SDL_Renderer* screen, int x, int y, int width, const SDL_Color& color)
{
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, text.c_str(), color, width);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(screen, textSurface);
	SDL_Rect dstRect = { x, y, textSurface->w, textSurface->h };
	SDL_RenderCopy(screen, texture, nullptr, &dstRect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(texture);
}

/**
 * Draws text centered on the screen.
 *
 * @param text The text to be drawn.
 * @param font The font to be used for rendering the text.
 * @param screen The SDL renderer to draw the text on.
 * @param x The x-coordinate of the top-left corner of the text.
 * @param y The y-coordinate of the top-left corner of the text.
 * @param width The width of the area where the text will be centered.
 * @param color The color of the text.
 */
void drawTextCentered(const std::string &text, TTF_Font *font, SDL_Renderer *screen, int x, int y, int width, const SDL_Color &color)
{
	int textWidth, textHeight;
	TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);
	int centeredX = x + (width - textWidth) / 2;
	drawText(text, font, screen, centeredX, y, color);
}