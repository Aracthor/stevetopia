#pragma once

#include "Components/PositionComponent.hpp"

#include "hatcher/Maths/glm_pure.hpp"

namespace TransformationHelper
{

glm::mat4 ModelFromComponents(const PositionComponent& position2D);

} // namespace TransformationHelper
