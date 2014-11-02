#pragma once

#include "Effect.h"

#include <stdint.h>
#include <glm/glm.hpp>

struct LineRenderer;

void LineRenderer_Create(LineRenderer* lr, uint32_t maxLines = 1000, bool isStatic = false, Effect* differentEffect = nullptr);
void LineRenderer_Destroy(LineRenderer* lr);

bool LineRenderer_AddLine(LineRenderer* lr, const float &x0, const float &y0, const float &z0, const float &x1, const float &y1, const float &z1, const uint32_t &colour);
bool LineRenderer_AddLine(LineRenderer* lr, const glm::vec3 &p0, const glm::vec3 &p1, const uint32_t &colour);
void LineRenderer_Render(LineRenderer* lr, glm::mat4 wvp);
