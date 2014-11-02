#pragma once
#include "Effect.h"

#include "ParticleSettings.h"
#include "Camera.h"
#include "Texture.h"

struct ParticleEffect : public Effect
{
  GLint vsViewportScaleIndex;
  GLint vsTimeIndex;
  GLint vsDepthIndex;

  GLint vsDuration;
  GLint vsDurationRandomness;
  GLint vsGravity;
  GLint vsEndVelocity;
  GLint vsMinColour;
  GLint vsMaxColour;
  GLint vsRotateSpeed;
  GLint vsStartSize;
  GLint vsEndSize;
};

void ParticleEffect_CreateFromFile(ParticleEffect* pe, const char* vertexFile, const char* fragmentFile);
void ParticleEffect_Destroy(ParticleEffect* pe);

void ParticleEffect_ApplyRenderer(ParticleEffect* pe, Camera* cam, GLuint depthMap, float aspectRatio);
void ParticleEffect_ApplyParticleSystem(ParticleEffect* pe, ParticleSettings &settings, float currentTime);