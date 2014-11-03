#ifndef BINARYREADERX_H
#define BINARYREADERX_H

#include <iostream>
#include <fstream>

struct BinaryReaderX
{
  char* data;
  int position;
  bool myMemory;
};

static void BRX_Create(BinaryReaderX* brx, char* data)
{
	brx->position = 0;
	brx->data = data;
  brx->myMemory = false;
}

static void BRX_Destroy(BinaryReaderX* brx)
{
  if (brx->myMemory)
    delete brx->data;

  brx->myMemory = false;
	brx->position = 0;
  brx->data = nullptr;
}

static bool BRX_LoadFile(BinaryReaderX* brx, char* filename)
{
  if (brx->data != nullptr)
    return false;

  brx->myMemory = true;
  std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
  std::streampos size;

  //Open file and read data into memory
  if (file.is_open()) {
    size = file.tellg();
    brx->data = new char[(unsigned int)size];

    file.seekg(0, std::ios::beg);
    file.read(brx->data, size);
    file.close();
  }
  else
  {
    return false;
  }

  return true;
}

static int BRX_ReadInt(BinaryReaderX* brx)
{
  int* p = (int*)&brx->data[brx->position];
  brx->position += 4;
  return *p;
}

static float BRX_ReadFloat(BinaryReaderX* brx)
{
  float* p = (float*)&brx->data[brx->position];
  brx->position += 4;
  return *p;
}

static char BRX_ReadByte(BinaryReaderX* brx)
{
  char* p = (char*)&brx->data[brx->position];
  brx->position++;
  return *p;
}

static short BRX_ReadShort(BinaryReaderX* brx)
{
  short* p = (short*)&brx->data[brx->position];
  brx->position += 2;
  return *p;
}

static unsigned short BRX_ReadUnsignedShort(BinaryReaderX* brx)
{
  unsigned short* p = (unsigned short*)&brx->data[brx->position];
  brx->position += 2;
  return *p;
}

static char* BRX_ReadCharString(BinaryReaderX* brx)
{
	short length = BRX_ReadShort(brx);

  unsigned short* p = (unsigned short*)&brx->data[brx->position];
		
	char* temp = new char[length+1];
	memcpy(temp, p, length);
	temp[length] = 0;

  brx->position += length;
		
	return temp;
}

static void BRX_ReadNullTerminatedString(BinaryReaderX* brx, char** ppString)
{
  char temp;
  int startPoint = brx->position;
  int endPoint = brx->position;

  temp = brx->data[brx->position];

  while (temp != 0)
  {
    endPoint++;
    temp = brx->data[endPoint];
  }

  *ppString = new char[endPoint - startPoint + 1];

  for (int i = startPoint; i < endPoint+1; i++)
  {
    (*ppString)[i - startPoint] = brx->data[i];
  }

  brx->position = endPoint + 1;
}

static std::string BRX_ReadString(BinaryReaderX* brx)
{
	short length = BRX_ReadShort(brx);

  unsigned short* p = (unsigned short*)&brx->data[brx->position];
		
	char* temp = new char[length+1];
	memcpy(temp, p, length);
	temp[length] = 0;

	std::string r = std::string(temp);

  brx->position += length;
		
	return r;
}

static void BRX_ReadAhead(BinaryReaderX* brx, int numBytes)
{
  brx->position += numBytes;
}

#endif