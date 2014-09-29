#include "Camera.h"


Camera::Camera(void) {
	
}


Camera::~Camera(void) {

}

void Camera::CreatePerspectiveProjection(float width, float height, float angle, float near, float far) {
	this->Projection = glm::perspectiveFov<float>(angle, width, height, near, far);
}

void Camera::CreateOrthographicProjection(float width, float height, float near, float far) {
	this->Projection = glm::ortho<float>(-width/2, width/2, height/2, -height/2, near, far);
}

void Camera::CreateOrthographicProjection(int width, int height, int near, int far) {
  if ((width & 1) > 0 && (height&1) > 0)
    this->Projection = glm::ortho<float>(-(width - 1) / 2, (width - 1) / 2 + 1, (height - 1) / 2 + 1, -(height - 1) / 2, near, far);
  else if ((height & 1) > 0)
    this->Projection = glm::ortho<float>(-width / 2, width / 2, (height - 1) / 2 + 1, -(height - 1) / 2, near, far);
  else if ((width & 1) > 0)
    this->Projection = glm::ortho<float>(-(width - 1) / 2 + 1, (width - 1) / 2, height / 2, -height / 2, near, far);
  else
    this->Projection = glm::ortho<float>(-width / 2, width / 2, height / 2, -height / 2, near, far);
}