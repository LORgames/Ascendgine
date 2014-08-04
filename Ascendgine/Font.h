#pragma once

#include "QuadRenderer.h"
#include "Texture.h"

#include <stdint.h>

struct FChar;

class Font
{
private:
  QuadRenderer *renderer;
  Texture* texture;

  short fontSize;     //The Font size
  char fontFlags;     //Some flags
  char* fontName;

  uint16_t lineHeight;  //This is the distance in pixels between each line of text.
  uint16_t base;        //The number of pixels from the absolute top of the line to the base of the characters.

  bool validFont;

  int totalCharacters;
  FChar* characters;

  int GetIDOfCharacter(int c);
public:
  Font(char* filename);
  ~Font();

  void DrawString(char* str, int x, int y);
};

