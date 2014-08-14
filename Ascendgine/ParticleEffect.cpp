#include "ParticleEffect.h"


ParticleEffect::ParticleEffect(const char* vertexFile, const char* fragmentFile) : Effect(vertexFile, fragmentFile)
{
  vsViewportScaleIndex = GetUniformID("ViewportScale");
  vsTimeIndex = GetUniformID("CurrentTime");

  vsDuration = GetUniformID("Duration");
  vsDurationRandomness = GetUniformID("DurationRandomness");
  vsGravity = GetUniformID("Gravity");

  vsEndVelocity = GetUniformID("EndVelocity");
  vsMinColour = GetUniformID("MinColour");
  vsMaxColour = GetUniformID("MaxColour");

  vsRotateSpeed = GetUniformID("RotateSpeed");

  vsStartSize = GetUniformID("StartSize");
  vsEndSize = GetUniformID("EndSize");

  vsDepthIndex = GetUniformID("DepthMap");
}


ParticleEffect::~ParticleEffect()
{

}

void ParticleEffect::ApplyRenderer(Camera* cam, GLuint depthMap, float aspectRatio, float currentTime)
{
  this->Apply(cam);

  glActiveTexture(GL_TEXTURE0 + 1);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glUniform1i(vsDepthIndex, 1);
  
  glUniform2f(vsViewportScaleIndex, 0.5f / aspectRatio, -0.5f);
  glUniform1f(vsTimeIndex, currentTime);
}

void ParticleEffect::ApplyParticleSystem(ParticleSettings &settings)
{
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, settings.TextureID);
  glUniform1i(psDiffuse, 0);

  glUniform1f(vsDuration, settings.Duration);
  glUniform1f(vsDurationRandomness, settings.DurationRandomness);
  glUniform3f(vsGravity, settings.Gravity.x, settings.Gravity.y, settings.Gravity.z);
  glUniform1f(vsEndVelocity, settings.EndVelocity);

  glUniform4f(vsMinColour, settings.MinColor[0], settings.MinColor[1], settings.MinColor[2], settings.MinColor[3]);
  glUniform4f(vsMinColour, settings.MaxColor[0], settings.MaxColor[1], settings.MaxColor[2], settings.MaxColor[3]);

  glUniform2f(vsRotateSpeed, settings.MinRotateSpeed, settings.MaxRotateSpeed);
  glUniform2f(vsStartSize, settings.MinStartSize, settings.MaxStartSize);
  glUniform2f(vsEndSize, settings.MinEndSize, settings.MaxEndSize);
}