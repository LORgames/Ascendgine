#include "Font.h"

#include "BinaryReaderX.h"
#include "StringHelper.h"

struct FChar
{
  uint32_t id;
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  int16_t xoffset;
  int16_t yoffset;
  int16_t xadvance;
};

void Font_Create(Font* font, char* filename)
{
  font->validFont = false;

  BinaryReaderX file;
  BRX_Create(&file, nullptr);
  if (!BRX_LoadFile(&file, filename))
  {
    printf_s("Could not load font! '%s'", filename);
    return;
  }

  int version = BRX_ReadInt(&file);

  if (version != 0x03464D42)  //Validate header "BMF3"
  {
    printf_s("Sorry, '%s' does not appear to be a font file! %d", filename, version);
    return;
  }

  char blockType;
  int blockSize;

  while (true)
  {
    blockType = BRX_ReadByte(&file);
    blockSize = BRX_ReadInt(&file);
    printf("Reading block type=%d, size=%d\n", blockType, blockSize);

    if (blockType == 1) //Block 1: Info
    {
      font->fontSize = BRX_ReadShort(&file);
      font->fontFlags = BRX_ReadByte(&file);
      BRX_ReadAhead(&file, 11); //Skip the rest of the info
      BRX_ReadNullTerminatedString(&file, &font->fontName);

      printf("Loaded font '%s' at size %d\n", font->fontName, font->fontSize);
    }
    else if (blockType == 2)  //Block 2: Common
    {
      font->lineHeight = BRX_ReadUnsignedShort(&file);
      font->base = BRX_ReadUnsignedShort(&file);
      BRX_ReadAhead(&file, 4);  //Skip some texture information
      uint16_t pages = BRX_ReadUnsignedShort(&file);

      if (pages != 1)
      {
        printf_s("Font '%s' failed to load because its on %s pages, export again as 1 page!\n", font->fontName, pages);
        return;
      }

      char flags = BRX_ReadByte(&file);       //bits 0-6: reserved, bit 7: packed
      char alphaChnl = BRX_ReadByte(&file);

      if (((flags & 0x40) > 0) && (alphaChnl != 1))
      {
        //TODO: Add support for the other channels
        printf_s("Font '%s' could not load as it isn't monochrome! (%d)\n", font->fontName, alphaChnl);
        return;
      }

      BRX_ReadAhead(&file, 3);
    }
    else if (blockType == 3) //Block 3: Pages
    {
      char *textureName;
      BRX_ReadNullTerminatedString(&file, &textureName);

      printf_s("Font '%s' uses texture '%s'\n", font->fontName, textureName);
      char pathBuffer[512];

      GetPathFromString(filename, pathBuffer);

      font->texture = new Texture();
      font->texture->LoadTextureFromPath(textureName, pathBuffer);

      delete[] textureName;
    }
    else if (blockType == 4) //Block 4: Characters
    {
      font->totalCharacters = blockSize / 20;
      font->characters = new FChar[font->totalCharacters];

      printf_s("Found %d characters in font '%s'.\n", font->totalCharacters, font->fontName);

      for (int i = 0; i < font->totalCharacters; i++)
      {
        font->characters[i].id = BRX_ReadInt(&file);
        font->characters[i].x = BRX_ReadUnsignedShort(&file);
        font->characters[i].y = BRX_ReadUnsignedShort(&file);
        font->characters[i].width = BRX_ReadUnsignedShort(&file);
        font->characters[i].height = BRX_ReadUnsignedShort(&file);
        font->characters[i].xoffset = BRX_ReadShort(&file);
        font->characters[i].yoffset = BRX_ReadShort(&file);
        font->characters[i].xadvance = BRX_ReadShort(&file);
        BRX_ReadAhead(&file, 2);
      }

      break;
    }
    else if (blockType == 5) //Block 5: Kerning Pairs
    {
      BRX_ReadAhead(&file, blockSize);
    }
  }

  font->renderer = new QuadRenderer(font->texture);
  font->validFont = true;
}

void Font_Destroy(Font* font)
{
  delete font->renderer;
  delete font->texture;

  delete font->fontName;
  delete[] font->characters;
}

int GetIDOfCharacter(Font* font, int c)
{
  //TODO: Make this a binary search tree
  //Alternatively, add a usage amount counter to the letters, inc when used and swap the character order so more used characters are at the front
  for (int i = 0; i < font->totalCharacters; i++)
  {
    if (font->characters[i].id == c)
      return i;
  }

  return -1;
}

void Font_DrawString(Font* font, char* str, int x, int y, uint32_t colour)
{
  float rX = (float)x;
  float rY = (float)y;
  int len = (int)strlen(str);

  int id;

  for (int i = 0; i < len; i++)
  {
    if (str[i] != 10)
    {
      id = GetIDOfCharacter(font, str[i]);

      if (id != -1)
      {
        font->renderer->AddQuadToRender(rX + font->characters[id].xoffset, rY + font->characters[id].yoffset, font->characters[id].x, font->characters[id].y, font->characters[id].width, font->characters[id].height);
        rX += font->characters[id].xadvance;
      }
      else
      {
        printf_s("'%s' cannot represent character %d.\n", font->fontName, str[i]);
      }
    }
    else
    {
      //Newline
      rX = (float)x;
      rY += font->lineHeight;
    }
  }

  font->renderer->Render();
}