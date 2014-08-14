#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ParticleEffect.h"
#include "ParticleSettings.h"

struct ParticleVertex
{
  float Corner[2];   // Stores which corner of the particle quad this vertex represents.
  float Position[3]; // Stores the starting position of the particle.
  float Velocity[3]; // Stores the starting velocity of the particle.
  float Random[4];   // Four random values, used to make each particle look slightly different.
  float Time;        // The time (in seconds) at which this particle was created.
};

class ParticleSystem
{
private:
  ParticleSettings settings;
  ParticleEffect* particleEffect;

  GLuint vaoID;
  GLuint vertexBufferID;
  GLuint indexBufferID;

  ParticleVertex* particles;
  int* indices;

  int firstActiveParticle;
  int firstNewParticle;
  int firstFreeParticle;
  int firstRetiredParticle;

  float currentTime;
  int drawCounter;

  //Functions
  void FreeRetiredParticles();
  void RetireActiveParticles();
  void AddNewParticlesToVertexBuffer();
protected:
  virtual void InitializeSettings(ParticleSettings &settings) = 0;
public:
  ParticleSystem();
  ~ParticleSystem();

  void Initialize(ParticleEffect* effect);

  void SetCamera(glm::mat4x4 view, glm::mat4x4 projection);
  bool AddParticle(glm::vec3 position, glm::vec3 velocity);

  void Update(float dt);
  void Render();
};

