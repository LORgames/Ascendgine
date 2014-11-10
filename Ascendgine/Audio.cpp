#include "Audio.h"
#include <limits.h>

struct AudioObject
{
  int noObj;
};

void Audio_Init()
{

}

void Audio_Destroy()
{

}

void Audio_Update(float dt)
{

}

void Audio_Play(char* filename, AudioObject* pAudioObject, float volume /*= 0.5f*/, bool loop /*= false*/)
{

}

void Audio_Cleanup(AudioObject* pAudioObject)
{

}

bool Audio_IsPlaying(AudioObject* pAudioObject)
{
  return false;
}

void Audio_StopPlaying(AudioObject* pAudioObject)
{

}