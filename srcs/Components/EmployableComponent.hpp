#pragma once

#include <optional>

#include "hatcher/Entity.hpp"

using namespace hatcher;

struct EmployableComponent
{
    std::optional<Entity> employer;
};
