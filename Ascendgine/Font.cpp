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

  BinaryReaderX file = BinaryReaderX(nullptr);
  if (!file.LoadFile(filename))
  {
    printf_s("Could not load font! '%s'", filename);
    return;
  }

  int version = file.ReadInt();

  if (version != 0x03464D42)
  {
    printf_s("Sorry, '%s' does not appear to be a font file! %d", filename, version);
    return;
  }

  char blockType;
  int blockSize;

  while (true)
  {
    blockType = file.ReadByte();
    blockSize = file.ReadInt();
    printf("Reading block type=%d, size=%d\n", blockType, blockSize);

    if (blockType == 1) //Block 1: Info
    {
      font->fontSize = file.ReadShort();
      font->fontFlags = file.ReadByte();
      file.ReadAhead(11); //Skip the rest of the info
      file.ReadNullTerminatedString(&font->fontName);

      printf("Loaded font '%s' at size %d\n", font->fontName, font->fontSize);
    }
    else if (blockType == 2)  //Block 2: Common
    {
      font->lineHeight = file.ReadUnsignedShort();
      font->base = file.ReadUnsignedShort();
      file.ReadAhead(4);  //Skip some texture information
      uint16_t pages = file.ReadUnsignedShort();

      if (pages != 1)
      {
        printf_s("Font '%s' failed to load because its on %s pages, export again as 1 page!\n", font->fontName, pages);
        return;
      }

      char flags = file.ReadByte();       //bits 0-6: reserved, bit 7: packed
      char alphaChnl = file.ReadByte();

      if (((flags & 0x40) > 0) && (alphaChnl != 1))
      {
        //TODO: Add support for the other channels
        printf_s("Font '%s' could not load as it isn't monochrome! (%d)\n", font->fontName, alphaChnl);
        return;
      }

      file.ReadAhead(3);
    }
    else if (blockType == 3) //Block 3: Pages
    {
      char *textureName;
      file.ReadNullTerminatedString(&textureName);

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
        font->characters[i].id = file.ReadInt();
        font->characters[i].x = file.ReadUnsignedShort();
        font->characters[i].y = file.ReadUnsignedShort();
        font->characters[i].width = file.ReadUnsignedShort();
        font->characters[i].height = file.ReadUnsignedShort();
        font->characters[i].xoffset = file.ReadShort();
        font->characters[i].yoffset = file.ReadShort();
        font->characters[i].xadvance = file.ReadShort();
        file.ReadAhead(2);
      }

      break;
    }
    else if (blockType == 5) //Block 5: Kerning Pairs
    {
      file.ReadAhead(blockSize);
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