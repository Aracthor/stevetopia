#pragma once

#include <vector>

#include "hatcher/Entity.hpp"
#include "hatcher/Maths/glm_pure.hpp"

#include "ActionPlanningComponent.hpp"

namespace hatcher
{
class DataLoader;
class DataSaver;
} // namespace hatcher

using namespace hatcher;

struct BusinessComponent
{
    glm::vec2 storagePosition;
    ActionPlanningComponent::EAgenda agenda;
    std::vector<Entity> employees;
};

void operator<<(DataSaver& saver, const BusinessComponent& component);
void operator>>(DataLoader& loader, BusinessComponent& component);
