#pragma once
#include "mesh.h"
class Quad: public Mesh {
	public:
		Quad(RenderMaterial* _mat) : Mesh(_mat) {

		}

		~Quad(void) {

		}
};

