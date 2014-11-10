#pragma once

#include <glm/glm.hpp>

struct Camera
{
  glm::mat4 Model;
  glm::mat4 View;
  glm::mat4 Projection;
  
  glm::vec3 Position;
};

void Camera_CreatePerspectiveProjection(Camera* camera, float width, float height, float angle, float near, float far);

void Camera_CreateOrthographicProjection(Camera* camera, float width, float height, float near, float far);
void Camera_CreateOrthographicProjection(Camera* camera, int width, int height, int near, int far);