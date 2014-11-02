#include "ParticleEffect.h"


void ParticleEffect_CreateFromFile(ParticleEffect* pe, const char* vertexFile, const char* fragmentFile)
{
  Effect_CreateFromFile(pe, vertexFile, fragmentFile);

  pe->vsViewportScaleIndex = Effect_GetUniformID(pe, "ViewportScale");
  pe->vsTimeIndex = Effect_GetUniformID(pe, "CurrentTime");

  pe->vsDuration = Effect_GetUniformID(pe, "Duration");
  pe->vsDurationRandomness = Effect_GetUniformID(pe, "DurationRandomness");
  pe->vsGravity = Effect_GetUniformID(pe, "Gravity");

  pe->vsEndVelocity = Effect_GetUniformID(pe, "EndVelocity");
  pe->vsMinColour = Effect_GetUniformID(pe, "MinColour");
  pe->vsMaxColour = Effect_GetUniformID(pe, "MaxColour");

  pe->vsRotateSpeed = Effect_GetUniformID(pe, "RotateSpeed");

  pe->vsStartSize = Effect_GetUniformID(pe, "StartSize");
  pe->vsEndSize = Effect_GetUniformID(pe, "EndSize");

  pe->vsDepthIndex = Effect_GetUniformID(pe, "DepthMap");
}


void ParticleEffect_Destroy(ParticleEffect* pe)
{
  Effect_Destroy(pe);
}

void ParticleEffect_ApplyRenderer(ParticleEffect* pe, Camera* cam, GLuint depthMap, float aspectRatio)
{
  Effect_Apply(pe, cam);

  glActiveTexture(GL_TEXTURE0 + 1);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glUniform1i(pe->vsDepthIndex, 1);
  
  glUniform2f(pe->vsViewportScaleIndex, 1.f / aspectRatio, -1.f);
}

void ParticleEffect_ApplyParticleSystem(ParticleEffect* pe, ParticleSettings &settings, float currentTime)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, settings.TextureID);
  glUniform1i(pe->psDiffuse, 0);
  glUniform1f(pe->vsTimeIndex, currentTime);

  glUniform1f(pe->vsDuration, settings.Duration);
  glUniform1f(pe->vsDurationRandomness, settings.DurationRandomness);
  glUniform3f(pe->vsGravity, settings.Gravity.x, settings.Gravity.y, settings.Gravity.z);
  glUniform1f(pe->vsEndVelocity, settings.EndVelocity);

  glUniform4f(pe->vsMinColour, settings.MinColor[0], settings.MinColor[1], settings.MinColor[2], settings.MinColor[3]);
  glUniform4f(pe->vsMinColour, settings.MaxColor[0], settings.MaxColor[1], settings.MaxColor[2], settings.MaxColor[3]);

  glUniform2f(pe->vsRotateSpeed, settings.MinRotateSpeed, settings.MaxRotateSpeed);
  glUniform2f(pe->vsStartSize, settings.MinStartSize, settings.MaxStartSize);
  glUniform2f(pe->vsEndSize, settings.MinEndSize, settings.MaxEndSize);
}