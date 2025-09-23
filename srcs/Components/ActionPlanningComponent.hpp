#pragma once

#include <optional>

#include "hatcher/Entity.hpp"

using namespace hatcher;

struct ActionPlanningComponent
{
    enum class EAgenda : unsigned int
    {
        Derp,
        Lumberjack,
    };

    EAgenda agenda = EAgenda::Derp;
    std::optional<int> currentActionIndex;
    std::optional<Entity> lockedEntity;
};
