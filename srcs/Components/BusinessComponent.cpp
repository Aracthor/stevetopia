#include "Components/BusinessComponent.hpp"

#include "hatcher/DataLoader.hpp"
#include "hatcher/DataSaver.hpp"

void operator<<(DataSaver& saver, const BusinessComponent& component)
{
    saver << component.storagePosition;
    saver << component.agenda;
    saver << component.employees;
}

void operator>>(DataLoader& loader, BusinessComponent& component)
{
    loader >> component.storagePosition;
    loader >> component.agenda;
    loader >> component.employees;
}
