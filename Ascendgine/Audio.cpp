#include "Audio.h"
#include <limits.h>
#include <FMOD\fmod.hpp>

struct AudioObject
{
  FMOD::Channel* sound;
};

FMOD::System* system;

void Audio_Init()
{
  FMOD::System_Create(&system);
  
  if (system)
  {
    system->init(32, FMOD_INIT_NORMAL, 0);
  }
}

void Audio_Destroy()
{
  if (system)
    system->release();
}

void Audio_Update(float dt)
{
  if (system)
    system->update();
}

void Audio_Play(char* filename, AudioObject* pAudioObject, float volume /*= 0.5f*/, bool loop /*= false*/)
{
  pAudioObject = new AudioObject();
  pAudioObject->sound = nullptr;

  FMOD::Sound* sound;
  system->createSound(filename, FMOD_SOFTWARE, 0, &sound);
  sound->setLoopCount(INT_MAX * (int)loop);

  system->playSound(FMOD_CHANNEL_FREE, sound, false, &pAudioObject->sound);
  pAudioObject->sound->setVolume(volume);
}

void Audio_Cleanup(AudioObject* pAudioObject)
{
  pAudioObject->sound->stop();
  FMOD::Sound* sound;
  pAudioObject->sound->getCurrentSound(&sound);
  sound->release();
}

bool Audio_IsPlaying(AudioObject* pAudioObject)
{
  bool success = false;
  if (pAudioObject == nullptr || pAudioObject->sound == nullptr)
    return success;
  pAudioObject->sound->isPlaying(&success);
  return success;
}

void Audio_StopPlaying(AudioObject* pAudioObject)
{
  if (pAudioObject == nullptr || pAudioObject->sound == nullptr)
    return;
  pAudioObject->sound->stop();
}