#pragma once

#include "QuadRenderer.h"
#include "Texture.h"

#include <stdint.h>

struct FChar;
struct Font;

void Font_Create(Font** ppFont, char* filename, uint32_t maxCharactersSimulatenously = 512);
void Font_Destroy(Font* font);

//Draw string methods
void Font_DrawString(Font* font, char* str, int x, int y, uint32_t colour);

void Font_AddString(Font* font, char* str, float x, float y, uint32_t colour);
//void Font_AddStaticString(Font* font, char* str, int x, int t, uint32_t colour);  //This function 'bakes' a string into the list so every time you run the render function this will be drawn as well
void Font_Render(Font* font);
void Font_UIResize(Font* font, int width, int height);

void Font_ResizeAll(int width, int height);