#pragma once

#include <iostream>
#include <fstream>

class BinaryReaderX {
private:
	int position;
	char* data;
  bool myMemory;

public:
	BinaryReaderX(char* data)
  {
		this->position = 0;
		this->data = data;
    myMemory = false;
	}

	~BinaryReaderX(void)
  {
    if (myMemory)
      delete data;

    myMemory = false;
		position = 0;
    data = nullptr;
	}

  bool LoadFile(char* filename)
  {
    if (data != nullptr)
      return false;

    myMemory = true;
    std::ifstream file(filename, std::ios::in | std::ios::binary | std::ios::ate);
    std::streampos size;

    //Open file and read data into memory
    if (file.is_open()) {
      size = file.tellg();
      data = new char[(unsigned int)size];

      file.seekg(0, std::ios::beg);
      file.read(data, size);
      file.close();
    }
    else
    {
      return false;
    }

    return true;
  }

	int ReadInt(void) {
		int* p = (int*)&data[position]; position += 4; return *p;
	}

	float ReadFloat(void)
  {
		float* p = (float*)&data[position]; position += 4; return *p;
	}

	char ReadByte(void)
  {
		char* p = (char*)&data[position]; position++; return *p;
	}

	unsigned short ReadShort(void)
  {
		short* p = (short*)&data[position]; position += 2; return *p;
	}

	unsigned short ReadUnsignedShort(void)
  {
		unsigned short* p = (unsigned short*)&data[position]; position += 2; return *p;
	}

	char* ReadCharString(void)
  {
		short length = ReadShort();

		unsigned short* p = (unsigned short*)&data[position];
		
		char* temp = new char[length+1];
		memcpy(temp, p, length);
		temp[length] = 0;

		position += length;
		
		return temp;
	}

	std::string ReadString(void)
  {
		short length = ReadShort();

		unsigned short* p = (unsigned short*)&data[position];
		
		char* temp = new char[length+1];
		memcpy(temp, p, length);
		temp[length] = 0;

		std::string r = std::string(temp);

		position += length;
		
		return r;
	}
};

