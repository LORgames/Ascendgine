#include "Camera.h"


Camera::Camera(void) {
	
}


Camera::~Camera(void) {

}

void Camera::CreatePerspectiveProjection(int width, int height, float angle, float near, float far) {
	this->Projection = glm::perspectiveFov<float>(angle, width, height, near, far);
}

void Camera::CreateOrthographicProjection(int width, int height, float near, float far) {
	this->Projection = glm::ortho<float>(-width/2, width/2, height/2, -height/2, near, far);
}