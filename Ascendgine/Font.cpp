#include "Font.h"

#include "BinaryReaderX.h"

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

Font::Font(char* filename)
{
  validFont = false;

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
      fontSize = file.ReadShort();
      fontFlags = file.ReadByte();
      file.ReadAhead(11); //Skip the rest of the info
      file.ReadNullTerminatedString(&fontName);

      printf("Loaded font '%s' at size %d\n", fontName, fontSize);
    }
    else if (blockType == 2)  //Block 2: Common
    {
      lineHeight = file.ReadUnsignedShort();
      base = file.ReadUnsignedShort();
      file.ReadAhead(4);  //Skip some texture information
      uint16_t pages = file.ReadUnsignedShort();

      if (pages != 1)
      {
        printf_s("Font '%s' failed to load because its on %s pages, export again as 1 page!\n", fontName, pages);
        return;
      }

      char flags = file.ReadByte();       //bits 0-6: reserved, bit 7: packed
      char alphaChnl = file.ReadByte();

      if (((flags & 0x40) > 0) && (alphaChnl != 1))
      {
        printf_s("Font '%s' could not load as it isn't monochrome! %d\n", fontName, alphaChnl);
        return;
      }

      file.ReadAhead(3);
    }
    else if (blockType == 3) //Block 3: Pages
    {
      char *textureName;
      file.ReadNullTerminatedString(&textureName);

      printf_s("Font '%s' uses texture '%s'\n", fontName, textureName);

      texture = new Texture();
      texture->LoadTexture("../assets/fonts/", textureName);

      delete[] textureName;
    }
    else if (blockType == 4) //Block 4: Characters
    {
      totalCharacters = blockSize / 20;
      characters = new FChar[totalCharacters];

      printf_s("Found %d characters in font '%s'.\n", totalCharacters, fontName);

      for (int i = 0; i < totalCharacters; i++)
      {
        characters[i].id = file.ReadInt();
        characters[i].x = file.ReadUnsignedShort();
        characters[i].y = file.ReadUnsignedShort();
        characters[i].width = file.ReadUnsignedShort();
        characters[i].height = file.ReadUnsignedShort();
        characters[i].xoffset = file.ReadShort();
        characters[i].yoffset = file.ReadShort();
        characters[i].xadvance = file.ReadShort();
        file.ReadAhead(2);
      }

      break;
    }
    else if (blockType == 5) //Block 5: Kerning Pairs
    {
      file.ReadAhead(blockSize);
    }
  }

  renderer = new QuadRenderer(texture);
  validFont = true;
}


Font::~Font()
{
  delete renderer;
  delete texture;

  delete fontName;
  delete[] characters;
}

int Font::GetIDOfCharacter(int c)
{
  for (int i = 0; i < totalCharacters; i++)
  {
    if (characters[i].id == c)
      return i;
  }

  return -1;
}

void Font::DrawString(char* str, int x, int y)
{
  float rX = (float)x;
  float rY = (float)y;
  int len = (int)strlen(str);

  int id;

  for (int i = 0; i < len; i++)
  {
    if (str[i] != 10)
    {
      id = GetIDOfCharacter(str[i]);

      if (id != -1)
      {
        renderer->AddQuadToRender(rX + characters[id].xoffset, rY + characters[id].yoffset, characters[id].x, characters[id].y, characters[id].width, characters[id].height);
        rX += characters[id].xadvance;
      }
      else
      {
        printf_s("'%s' cannot represent character %d.\n", fontName, str[i]);
      }
    }
    else
    {
      //Newline
      rX = (float)x;
      rY += lineHeight;
    }
  }
  renderer->Render();
}