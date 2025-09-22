#include "MovementComponent.hpp"

#include "hatcher/DataLoader.hpp"
#include "hatcher/DataSaver.hpp"

void operator<<(DataSaver& saver, const MovementComponent& component)
{
    saver << component.path;
}

void operator>>(DataLoader& loader, MovementComponent& component)
{
    loader >> component.path;
}
