#include "Font.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BinaryReaderX.h"
#include "StringHelper.h"
#include "EngineCore.h"

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

struct FontVertex
{
  float Position[2];
  float UVs[2];
  uint32_t Colour;
};

struct Font
{
  short fontSize;     //The Font size
  char fontFlags;     //Some flags
  char* fontName;

  uint16_t lineHeight;  //This is the distance in pixels between each line of text.
  uint16_t base;        //The number of pixels from the absolute top of the line to the base of the characters.

  int totalCharacters;
  FChar* characters;

  //QuadRendererStuff
  float textureWidth;
  float textureHeight;
  GLuint texID;

  GLuint vaoID;
  GLuint bufferID;
  GLuint indexBufferID;

  int* indices;
  FontVertex* vertices;
  int maxQuads;
  int usedQuads;
  int staticQuads;
};

Effect font_defaultShader = { 0 };
glm::mat4 font_proj = glm::ortho<float>(0, 1, 1, 0, -1.f, 1.f);

void Font_Create(Font** ppFont, char* filename, uint32_t maxCharactersSimulatenously)
{
  *ppFont = new Font;
  Font* font = *ppFont;

  memset(font, 0, sizeof(Font));

  BinaryReaderX file;
  BRX_Create(&file, nullptr);
  if (!BRX_LoadFile(&file, filename))
  {
    printf_s("Could not load font! '%s'", filename);
    goto epilogue;
  }

  int version = BRX_ReadInt(&file);

  if (version != 0x03464D42)  //Validate header "BMF3"
  {
    printf_s("Sorry, '%s' does not appear to be a font file! %d", filename, version);
    goto epilogue;
  }

  char blockType;
  int blockSize;

  //Load the font information
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
      
      int width, height;
      font->texID = Texture_LoadFromPath(textureName, pathBuffer, &width, &height);
      
      font->textureWidth = (float)width;
      font->textureHeight = (float)height;

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

  
  //Prepare CPU memory
  font->maxQuads = maxCharactersSimulatenously;
  
  font->vertices = new FontVertex[font->maxQuads * 4];
  font->indices = new int[font->maxQuads * 6];
  font->usedQuads = 0;

  for (int i = 0; i < font->maxQuads; i++)
  {
    font->indices[i * 6 + 0] = i * 4 + 0;
    font->indices[i * 6 + 1] = i * 4 + 1;
    font->indices[i * 6 + 2] = i * 4 + 2;
    font->indices[i * 6 + 3] = i * 4 + 0;
    font->indices[i * 6 + 4] = i * 4 + 2;
    font->indices[i * 6 + 5] = i * 4 + 3;
  }

  font->vertices = font->vertices;
  int totalVertices = font->maxQuads * 4;
  font->indices = font->indices;
  int totalIndices = font->maxQuads * 6;

  //Create the buffers on the GPU
  glGenVertexArrays(1, &font->vaoID);
  glBindVertexArray(font->vaoID);

  GLenum ErrorCheckValue = glGetError();
  const size_t BufferSize = sizeof(FontVertex)*totalVertices;
  const size_t VertexSize = sizeof(FontVertex);

  glGenBuffers(1, &font->bufferID);
  glBindBuffer(GL_ARRAY_BUFFER, font->bufferID);
  glBufferData(GL_ARRAY_BUFFER, BufferSize, font->vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT,         GL_FALSE, VertexSize, (GLvoid*)(0 * sizeof(float)));
  glVertexAttribPointer(1, 2, GL_FLOAT,         GL_FALSE, VertexSize, (GLvoid*)(2 * sizeof(float)));
  glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE,  VertexSize, (GLvoid*)(4 * sizeof(float)));  //BGRA

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  glGenBuffers(1, &font->indexBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->indexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*totalIndices, font->indices, GL_STATIC_DRAW);

  ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
    exit(-1);
  }

  //Load the shader
  if (font_defaultShader.id == 0)
  {
    Effect_CreateFromText(&font_defaultShader,
      "#version 330 core\n\n//Input Format\nlayout(location = 0) in vec2 vertexPosition_screenSpace;\nlayout(location = 1) in vec2 vertexTextureUVs;\nlayout(location = 2) in vec4 vertexColours;\n\n//Ouput Format\nout vec2 ex_TextureUVs;\nout vec4 ex_Colour;\n\n//Uniforms\nuniform mat4 Projection;\n\nvoid main()\n{\n  gl_Position = Projection * vec4(vertexPosition_screenSpace, 0.0, 1.0);\n  ex_TextureUVs = vertexTextureUVs;\n  ex_Colour = vertexColours;\n}",
      "#version 330 core\n\n//Other Stuff\nuniform sampler2D diffuse;\n\n//Input Format\nin vec2 ex_TextureUVs;\nin vec4 ex_Colour;\n\n//Output Format\nout vec4 out_Colour;\n\nvoid main()\n{\n  out_Colour = texture2D(diffuse, ex_TextureUVs);\n  out_Colour.rgb = ex_Colour.bgr;\n  out_Colour.a = out_Colour.a * ex_Colour.a;\n}");
  }

epilogue:
  BRX_Destroy(&file);
}

void Font_Destroy(Font* font)
{
  Texture_Destroy(font->texID);

  delete font->fontName;
  delete[] font->characters;

  delete font;
}

int GetIDOfCharacter(Font* font, int c)
{
  //TODO: Make this a binary search tree?
  //Alternatively, add a usage amount counter to the letters, inc when used and swap the character order so more used characters are at the front
  for (int i = 0; i < font->totalCharacters; i++)
  {
    if (font->characters[i].id == c)
      return i;
  }

  return -1;
}

bool Font_AddQuadToRender(Font* font, float x, float y, float texX, float texY, float scrW, float scrH, uint32_t colour)
{
  if (font->usedQuads == font->maxQuads)
    return false;

  float rW = scrW;
  float rH = scrH;

  font->vertices[font->usedQuads * 4 + 0].Position[0] = x;
  font->vertices[font->usedQuads * 4 + 0].Position[1] = y + scrH;
  font->vertices[font->usedQuads * 4 + 1].Position[0] = x;
  font->vertices[font->usedQuads * 4 + 1].Position[1] = y;
  font->vertices[font->usedQuads * 4 + 2].Position[0] = x + scrW;
  font->vertices[font->usedQuads * 4 + 2].Position[1] = y;
  font->vertices[font->usedQuads * 4 + 3].Position[0] = x + scrW;
  font->vertices[font->usedQuads * 4 + 3].Position[1] = y + scrH;

  font->vertices[font->usedQuads * 4 + 0].UVs[0] = (texX + 0)  / font->textureWidth;
  font->vertices[font->usedQuads * 4 + 0].UVs[1] = (texY + rH) / font->textureHeight;
  font->vertices[font->usedQuads * 4 + 1].UVs[0] = (texX + 0)  / font->textureWidth;
  font->vertices[font->usedQuads * 4 + 1].UVs[1] = (texY + 0)  / font->textureHeight;
  font->vertices[font->usedQuads * 4 + 2].UVs[0] = (texX + rW) / font->textureWidth;
  font->vertices[font->usedQuads * 4 + 2].UVs[1] = (texY + 0)  / font->textureHeight;
  font->vertices[font->usedQuads * 4 + 3].UVs[0] = (texX + rW) / font->textureWidth;
  font->vertices[font->usedQuads * 4 + 3].UVs[1] = (texY + rH) / font->textureHeight;

  font->vertices[font->usedQuads * 4 + 0].Colour = colour;
  font->vertices[font->usedQuads * 4 + 1].Colour = colour;
  font->vertices[font->usedQuads * 4 + 2].Colour = colour;
  font->vertices[font->usedQuads * 4 + 3].Colour = colour;

  font->usedQuads++;
  return true;
}

void Font_DrawString(Font* font, char* str, int x, int y, uint32_t colour)
{
  Font_AddString(font, str, (float)x, (float)y, colour);
  Font_Render(font);
}

void Font_AddString(Font* font, char* str, float x, float y, uint32_t colour)
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
        Font_AddQuadToRender(font, rX + font->characters[id].xoffset, rY + font->characters[id].yoffset, font->characters[id].x, font->characters[id].y, font->characters[id].width, font->characters[id].height, colour);
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
}

void Font_Render(Font* font)
{
  //Render (originally from QuadRenderer)
  Effect_Apply(&font_defaultShader);
  glUniformMatrix4fv(font_defaultShader.vsProjectionIndex, 1, GL_FALSE, glm::value_ptr(font_proj));

  //Engine_FontResized(window_width, window_height);

  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glBindVertexArray(font->vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, font->bufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, font->indexBufferID);

  //Upload if required
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(FontVertex) * 4 * font->usedQuads, font->vertices);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, font->texID);

  // Draw the triangles!
  glDrawElements(GL_TRIANGLES, font->usedQuads * 6, GL_UNSIGNED_INT, 0);

  font->usedQuads = 0;
}

void Engine_FontResized(int width, int height)
{
  font_proj = glm::ortho<float>(0.f, (float)width, (float)height, 0.f, -1.f, 1.f);
}