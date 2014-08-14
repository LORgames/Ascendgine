#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter(ParticleSystem* particleSystem, float particlesPerSecond, glm::vec3 initialPosition)
{
  this->particleSystem = particleSystem;
  timeBetweenParticles = 1.0f / particlesPerSecond;
  previousPosition = initialPosition;

  timeLeftOver = 0;
}

ParticleEmitter::~ParticleEmitter()
{

}

void ParticleEmitter::Update(float dt, glm::vec3 newPosition)
{
  if (dt > 0)
  {
    // Work out how fast we are moving.
    glm::vec3 velocity = (newPosition - previousPosition) / dt;

    // If we had any time left over that we didn't use during the
    // previous update, add that to the current elapsed time.
    float timeToSpend = timeLeftOver + dt;

    // Counter for looping over the time interval.
    float currentTime = -timeLeftOver;

    // Create particles as long as we have a big enough time interval.
    while (timeToSpend > timeBetweenParticles)
    {
      currentTime += timeBetweenParticles;
      timeToSpend -= timeBetweenParticles;

      // Work out the optimal position for this particle. This will produce
      // evenly spaced particles regardless of the object speed, particle
      // creation frequency, or game update rate.
      float mu = currentTime / dt;

      glm::vec3 position = previousPosition + (newPosition - previousPosition) * mu;

      // Create the particle.
      particleSystem->AddParticle(position, velocity);
    }

    // Store any time we didn't use, so it can be part of the next update.
    timeLeftOver = timeToSpend;
  }

  previousPosition = newPosition;
}