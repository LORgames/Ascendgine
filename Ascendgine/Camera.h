#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma once
class Camera {
	public:
		glm::mat4 Model, View, Projection;

		Camera(void);
		~Camera(void);

		void CreatePerspectiveProjection(int width, int height, float angle, float near, float far);
		void CreateOrthographicProjection(int width, int height, float near, float far);
};