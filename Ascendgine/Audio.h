#pragma once

struct AudioObject;

void Audio_Play(char* filename, AudioObject* pAudioObject);
void Audio_Cleanup(AudioObject* pAudioObject);

bool Audio_IsPlaying(AudioObject* pAudioObject);