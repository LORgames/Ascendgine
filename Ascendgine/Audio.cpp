#include "Audio.h"

#include <mpg123.h>

struct AudioObject
{
  
};

void Audio_Play(char* filename, AudioObject* pAudioObject)
{
  pAudioObject = new AudioObject();
}

void Audio_Cleanup(AudioObject* pAudioObject)
{
  
}

bool Audio_IsPlaying(AudioObject* pAudioObject)
{
  return false;
}