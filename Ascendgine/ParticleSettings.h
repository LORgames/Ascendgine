#pragma once

#include <stdint.h>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct ParticleSettings
{
  int TextureID;        // Name of the texture used by this particle system.
  int MaxParticles = 100;   // Maximum number of particles that can be displayed at one time.
  
  float Duration = 1.0;    // How long these particles will last.
  float DurationRandomness = 0; // If greater than zero, some particles will last a shorter time than others.
  
  // Controls how much particles are influenced by the velocity of the object
  // which created them. You can see this in action with the explosion effect,
  // where the flames continue to move in the same direction as the source
  // projectile. The projectile trail particles, on the other hand, set this
  // value very low so they are less affected by the velocity of the projectile.
  float EmitterVelocitySensitivity = 1;

  // Range of values controlling how much X and Z axis velocity to give each
  // particle. Values for individual particles are randomly chosen from somewhere
  // between these limits.
  float MinHorizontalVelocity = 0;
  float MaxHorizontalVelocity = 0;

  // Range of values controlling how much Y axis velocity to give each particle.
  // Values for individual particles are randomly chosen from somewhere between
  // these limits.
  float MinVerticalVelocity = 0;
  float MaxVerticalVelocity = 0;

  // Direction and strength of the gravity effect. Note that this can point in any
  // direction, not just down! The fire effect points it upward to make the flames
  // rise, and the smoke plume points it sideways to simulate wind.
  glm::vec3 Gravity = glm::vec3();

  // Controls how the particle velocity will change over their lifetime. If set
  // to 1, particles will keep going at the same speed as when they were created.
  // If set to 0, particles will come to a complete stop right before they die.
  // Values greater than 1 make the particles speed up over time.
  float EndVelocity = 1;

  // Range of values controlling the particle color and alpha. Values for
  // individual particles are randomly chosen from somewhere between these limits.
  float MinColor[4];// = { 1.f, 1.f, 1.f, 1.f };
  float MaxColor[4];// = { 1.f, 1.f, 1.f, 1.f };

  // Range of values controlling how fast the particles rotate. Values for
  // individual particles are randomly chosen from somewhere between these
  // limits. If both these values are set to 0, the particle system will
  // automatically switch to an alternative shader technique that does not
  // support rotation, and thus requires significantly less GPU power. This
  // means if you don't need the rotation effect, you may get a performance
  // boost from leaving these values at 0.
  float MinRotateSpeed = 0;
  float MaxRotateSpeed = 0;

  // Range of values controlling how big the particles are when first created.
  // Values for individual particles are randomly chosen from somewhere between
  // these limits.
  float MinStartSize = 100;
  float MaxStartSize = 100;

  // Range of values controlling how big particles become at the end of their
  // life. Values for individual particles are randomly chosen from somewhere
  // between these limits.
  float MinEndSize = 100;
  float MaxEndSize = 100;

  // Alpha blending settings.
  GLenum BlendState = GL_SRC_ALPHA;
};

