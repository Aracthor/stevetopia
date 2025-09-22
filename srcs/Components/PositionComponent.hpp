#pragma once

#include <glm/vec2.hpp>

struct PositionComponent
{
    glm::vec2 position = {0.f, 0.f};
    glm::vec2 orientation = {1.f, 0.f};
};
