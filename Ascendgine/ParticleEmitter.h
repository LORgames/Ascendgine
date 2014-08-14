//-----------------------------------------------------------------------------
// ParticleEmitter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

// Ported to OpenGL: Paul Fox

#include "ParticleSystem.h"
#include <glm\glm.hpp>

class ParticleEmitter
{
private:
    ParticleSystem* particleSystem;
    float timeBetweenParticles;
    glm::vec3 previousPosition;
    float timeLeftOver;
public:
  ParticleEmitter(ParticleSystem* particleSystem, float particlesPerSecond, glm::vec3 initialPosition);
  ~ParticleEmitter();

  void Update(float dt, glm::vec3 newPosition);
};
