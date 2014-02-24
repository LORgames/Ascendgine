#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
	public:
		glm::mat4 Model, View, Projection;

		Camera(void);
		~Camera(void);

		void CreatePerspectiveProjection(float width, float height, float angle, float near, float far);
		void CreateOrthographicProjection(float width, float height, float near, float far);
};