#pragma once
#include "Effect.h"

#include "ParticleSettings.h"
#include "Camera.h"
#include "Texture.h"

class ParticleEffect : public Effect
{
private:
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
public:
  ParticleEffect(const char* vertexFile, const char* fragmentFile);
  ~ParticleEffect();

  void ApplyRenderer(Camera* cam, GLuint depthMap, float aspectRatio);
  void ApplyParticleSystem(ParticleSettings &settings, float currentTime);
};