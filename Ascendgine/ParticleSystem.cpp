#include "ParticleSystem.h"

#include <stdlib.h>

ParticleSystem::ParticleSystem()
{
  firstActiveParticle = 0;
  firstNewParticle = 0;
  firstFreeParticle = 0;
  firstRetiredParticle = 0;

  currentTime = 0.f;
  drawCounter = 0;
}


ParticleSystem::~ParticleSystem()
{
  if (particles)
    delete[] particles;

  if (indices)
    delete[] indices;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &vertexBufferID);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &indexBufferID);
}

/// <summary>
/// Initializes the component.
/// </summary>
void ParticleSystem::Initialize(ParticleEffect* effect) {
  particleEffect = effect;
  InitializeSettings(settings);

  // Allocate the particle array, and fill in the corner fields (which never change).
  particles = new ParticleVertex[settings.MaxParticles * 4];
  indices = new int[settings.MaxParticles * 6];

  for (int i = 0; i < settings.MaxParticles; i++)
  {
    particles[i * 4 + 0].Corner[0] = -1.f;
    particles[i * 4 + 0].Corner[1] = -1.f;
    particles[i * 4 + 1].Corner[0] = 1.f;
    particles[i * 4 + 1].Corner[1] = -1.f;
    particles[i * 4 + 2].Corner[0] = 1.f;
    particles[i * 4 + 2].Corner[1] = 1.f;
    particles[i * 4 + 3].Corner[0] = -1.f;
    particles[i * 4 + 3].Corner[1] = 1.f;
  }

  for (int i = 0; i < settings.MaxParticles; i++) {
    indices[i * 6 + 0] = (i * 4 + 0);
    indices[i * 6 + 1] = (i * 4 + 1);
    indices[i * 6 + 2] = (i * 4 + 2);
    indices[i * 6 + 3] = (i * 4 + 0);
    indices[i * 6 + 4] = (i * 4 + 2);
    indices[i * 6 + 5] = (i * 4 + 3);
  }

  glGenVertexArrays(1, &vaoID);
  glBindVertexArray(vaoID);

  GLenum ErrorCheckValue = glGetError();
  const size_t BufferSize = sizeof(ParticleVertex)*(settings.MaxParticles*4);
  const size_t VertexSize = sizeof(ParticleVertex);

  glGenBuffers(1, &vertexBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, BufferSize, particles, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(0 * sizeof(float))); // Position
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(3 * sizeof(float))); // Corner
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(5 * sizeof(float))); // Velocity
  glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(8 * sizeof(float))); // Random
  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)(12* sizeof(float))); // Time

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);

  glGenBuffers(1, &indexBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*settings.MaxParticles*6, indices, GL_STATIC_DRAW);

  ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR) {
    fprintf(stderr, "ERROR: Could not create a VBO: %s \n", gluErrorString(ErrorCheckValue));
    exit(-1);
  }
}

/// <summary>
/// Updates the particle system.
/// </summary>
void ParticleSystem::Update(float dt) {
  currentTime += dt;

  RetireActiveParticles();
  FreeRetiredParticles();

  // If we let our timer go on increasing for ever, it would eventually
  // run out of floating point precision, at which point the particles
  // would render incorrectly. An easy way to prevent this is to notice
  // that the time value doesn't matter when no particles are being drawn,
  // so we can reset it back to zero any time the active queue is empty.

  if (firstActiveParticle == firstFreeParticle)
    currentTime = 0;

  if (firstRetiredParticle == firstActiveParticle)
    drawCounter = 0;
}


/// <summary>
/// Helper for checking when active particles have reached the end of
/// their life. It moves old particles from the active area of the queue
/// to the retired section.
/// </summary>
void ParticleSystem::RetireActiveParticles() {
  float particleDuration = settings.Duration;

  while (firstActiveParticle != firstNewParticle) {
    // Is this particle old enough to retire?
    // We multiply the active particle index by four, because each
    // particle consists of a quad that is made up of four vertices.
    float particleAge = currentTime - particles[firstActiveParticle * 4].Time;

    if (particleAge < particleDuration)
      break;

    // Remember the time at which we retired this particle.
    particles[firstActiveParticle * 4].Time = (float)drawCounter;

    // Move the particle from the active to the retired queue.
    firstActiveParticle++;

    if (firstActiveParticle >= settings.MaxParticles)
      firstActiveParticle = 0;
  }
}


/// <summary>
/// Helper for checking when retired particles have been kept around long
/// enough that we can be sure the GPU is no longer using them. It moves
/// old particles from the retired area of the queue to the free section.
/// </summary>
void ParticleSystem::FreeRetiredParticles() {
  while (firstRetiredParticle != firstActiveParticle) {
    // Has this particle been unused long enough that
    // the GPU is sure to be finished with it?
    // We multiply the retired particle index by four, because each
    // particle consists of a quad that is made up of four vertices.
    int age = drawCounter - (int)particles[firstRetiredParticle * 4].Time;

    // The GPU is never supposed to get more than 2 frames behind the CPU.
    // We add 1 to that, just to be safe in case of buggy drivers that
    // might bend the rules and let the GPU get further behind.
    if (age < 3)
      break;

    // Move the particle from the retired to the free queue.
    firstRetiredParticle++;

    if (firstRetiredParticle >= settings.MaxParticles)
      firstRetiredParticle = 0;
  }
}


/// <summary>
/// Draws the particle system.
/// </summary>
void ParticleSystem::Render() {
  particleEffect->ApplyParticleSystem(settings, currentTime);

  //Fix the states for light rendering
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glDepthMask(GL_TRUE);
  glDisable(GL_DEPTH_TEST);

  glBindVertexArray(vaoID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

  if (firstNewParticle != firstFreeParticle) {
    AddNewParticlesToVertexBuffer();
  }

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);
  glEnableVertexAttribArray(3);
  glEnableVertexAttribArray(4);

  // If there are any active particles, draw them now!
  if (firstActiveParticle != firstFreeParticle) {
    if (firstActiveParticle < firstFreeParticle) {
      glDrawElements(GL_TRIANGLES, (firstFreeParticle - firstActiveParticle) * 6, GL_UNSIGNED_INT, (void*)(firstActiveParticle * 6));
    }
    else
    {
      glDrawElements(GL_TRIANGLES, (settings.MaxParticles - firstActiveParticle) * 6, GL_UNSIGNED_INT, indices + (firstActiveParticle * 6));

      if (firstFreeParticle > 0)
        glDrawElements(GL_TRIANGLES, firstFreeParticle * 4, GL_UNSIGNED_INT, 0);
    }
  }

  glDisableVertexAttribArray(3);
  glDisableVertexAttribArray(4);

  drawCounter++;
}

/// <summary>
/// Adds a new particle to the system.
/// </summary>
bool ParticleSystem::AddParticle(glm::vec3 position, glm::vec3 velocity) {
  // Figure out where in the circular queue to allocate the new particle.
  int nextFreeParticle = firstFreeParticle + 1;

  if (nextFreeParticle >= settings.MaxParticles)
    nextFreeParticle = 0;

  // If there are no free particles, we just have to give up.
  if (nextFreeParticle == firstRetiredParticle)
    return false;

  // Adjust the input velocity based on how much
  // this particle system wants to be affected by it.
  velocity *= settings.EmitterVelocitySensitivity;

  // Add in some random amount of horizontal velocity.
  float horizontalVelocity = settings.MinHorizontalVelocity + (settings.MaxHorizontalVelocity - settings.MinHorizontalVelocity) * ((float)rand() / RAND_MAX);

  float horizontalAngle = ((float)rand() / RAND_MAX) * 6.28318f; //2PI

  velocity.x += horizontalVelocity * (float)glm::cos(horizontalAngle);
  velocity.z += horizontalVelocity * (float)glm::sin(horizontalAngle);

  // Add in some random amount of vertical velocity.
  velocity.y += settings.MinVerticalVelocity + (settings.MaxVerticalVelocity - settings.MinVerticalVelocity) * ((float)rand() / RAND_MAX);

  // Fill in the particle vertex structure.
  for (int i = 0; i < 4; i++)
  {
    particles[firstFreeParticle * 4 + i].Position[0] = position.x;
    particles[firstFreeParticle * 4 + i].Position[1] = position.y;
    particles[firstFreeParticle * 4 + i].Position[2] = position.z;
    particles[firstFreeParticle * 4 + i].Velocity[0] = velocity.x;
    particles[firstFreeParticle * 4 + i].Velocity[1] = velocity.y;
    particles[firstFreeParticle * 4 + i].Velocity[2] = velocity.z;
    particles[firstFreeParticle * 4 + i].Random[0] = ((float)rand() / RAND_MAX);
    particles[firstFreeParticle * 4 + i].Random[1] = ((float)rand() / RAND_MAX);
    particles[firstFreeParticle * 4 + i].Random[2] = ((float)rand() / RAND_MAX);
    particles[firstFreeParticle * 4 + i].Random[3] = ((float)rand() / RAND_MAX);
    particles[firstFreeParticle * 4 + i].Time = currentTime;
  }

  firstFreeParticle = nextFreeParticle;
  return true;
}

void ParticleSystem::AddNewParticlesToVertexBuffer()
{
  int stride = sizeof(ParticleVertex);

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ParticleVertex)*settings.MaxParticles*4, particles);

  /*if (firstNewParticle < firstFreeParticle)
  {
    // If the new particles are all in one consecutive range,
    // we can upload them all in a single call.
    glBufferSubData(GL_ARRAY_BUFFER, firstNewParticle * stride * 4, (firstFreeParticle - firstNewParticle) * 4 * stride, particles);

    //vertexBuffer.SetData(firstNewParticle * stride * 4, particles, firstNewParticle * 4, (firstFreeParticle - firstNewParticle) * 4, stride, SetDataOptions.NoOverwrite);
  }
  else
  {
    // If the new particle range wraps past the end of the queue
    // back to the start, we must split them over two upload calls.
    //vertexBuffer.SetData(firstNewParticle * stride * 4, particles, firstNewParticle * 4, (settings.MaxParticles - firstNewParticle) * 4, stride, SetDataOptions.NoOverwrite);

    glBufferSubData(GL_ARRAY_BUFFER, firstNewParticle * stride * 4, (settings.MaxParticles - firstNewParticle) * 4 * stride, particles);
    if (firstFreeParticle > 0)
    {
      glBufferSubData(GL_ARRAY_BUFFER, 0, firstFreeParticle * 4 * stride, particles);

      //vertexBuffer.SetData(0, particles, 0, firstFreeParticle * 4, stride, SetDataOptions.NoOverwrite);
    }
  }*/

  // Move the particles we just uploaded from the new to the active queue.
  firstNewParticle = firstFreeParticle;
}