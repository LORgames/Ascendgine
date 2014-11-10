#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Camera_CreatePerspectiveProjection(Camera* camera, float width, float height, float angle, float near, float far)
{
  camera->Projection = glm::perspectiveFov<float>(angle, width, height, near, far);
}

void Camera_CreateOrthographicProjection(Camera* camera, float width, float height, float near, float far)
{
  camera->Projection = glm::ortho<float>(-width / 2, width / 2, height / 2, -height / 2, near, far);
}

void Camera_CreateOrthographicProjection(Camera* camera, int width, int height, int near, int far)
{
  if ((width & 1) > 0 && (height&1) > 0)
    camera->Projection = glm::ortho<int>(-(width - 1) / 2, (width - 1) / 2 + 1, (height - 1) / 2 + 1, -(height - 1) / 2, near, far);
  else if ((height & 1) > 0)
    camera->Projection = glm::ortho<int>(-width / 2, width / 2, (height - 1) / 2 + 1, -(height - 1) / 2, near, far);
  else if ((width & 1) > 0)
    camera->Projection = glm::ortho<int>(-(width - 1) / 2 + 1, (width - 1) / 2, height / 2, -height / 2, near, far);
  else
    camera->Projection = glm::ortho<int>(-width / 2, width / 2, height / 2, -height / 2, near, far);
}