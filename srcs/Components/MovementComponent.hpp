#pragma once

#include <glm/vec2.hpp>
#include <vector>

namespace hatcher
{
class DataLoader;
class DataSaver;
} // namespace hatcher

using namespace hatcher;

struct MovementComponent
{
    std::vector<glm::vec2> path; // Reversed : last element is the next step.
};

void operator<<(DataSaver& saver, const MovementComponent& component);
void operator>>(DataLoader& loader, MovementComponent& component);
