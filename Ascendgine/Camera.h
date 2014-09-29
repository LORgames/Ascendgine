#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
	public:
		glm::mat4 Model, View, Projection;

    glm::vec3 Position;

		Camera(void);
		~Camera(void);

		void CreatePerspectiveProjection(float width, float height, float angle, float near, float far);
	
    void CreateOrthographicProjection(float width, float height, float near, float far);
    void CreateOrthographicProjection(int width, int height, int near, int far);
};