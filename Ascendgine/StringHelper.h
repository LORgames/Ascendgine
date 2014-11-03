#ifndef STRINGHELPER_H
#define STRINGHELPER_H 1

#include <string.h>

static void GetPathFromString(const char* filename, char* path)
{
  int strlength = (int)strlen(filename);

  int lastSlash = 0;

  for (int i = strlength; i >= 0; --i)
  {
    if (filename[i] == '\\' || filename[i] == '/')
    {
      lastSlash = i;
      break;
    }
  }

  for (int i = 0; i < lastSlash; i++)
  {
    path[i] = filename[i];
  }

  path[lastSlash] = 0;
}

#endif