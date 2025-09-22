#include "Components/ActionPlanningComponent.hpp"
#include "Components/BusinessComponent.hpp"

#include "hatcher/ComponentAccessor.hpp"
#include "hatcher/Updater.hpp"

#include "utils/EntityFinder.hpp"

using namespace hatcher;

namespace
{

bool IsBusiness(const ComponentAccessor* componentAccessor, Entity entity)
{
    ComponentReader<BusinessComponent> businesses = componentAccessor->ReadComponents<BusinessComponent>();
    return businesses[entity].has_value();
}

class BusinessUpdater final : public Updater
{
    void Update(IEntityManager* entityManager, ComponentAccessor* componentAccessor) override
    {
        ComponentWriter<BusinessComponent> businesses = componentAccessor->WriteComponents<BusinessComponent>();
        ComponentWriter<ActionPlanningComponent> plannings =
            componentAccessor->WriteComponents<ActionPlanningComponent>();

        for (int i = 0; i < componentAccessor->Count(); i++)
        {
            if (plannings[i])
            {
                ActionPlanningComponent& planning = *plannings[i];
                if (!planning.employer)
                {
                    const Entity entity(i);
                    Entity businessEntity = FindNearestEntity(componentAccessor, entity, IsBusiness);
                    if (businessEntity != Entity::Invalid())
                    {
                        BusinessComponent& business = *businesses[businessEntity];
                        business.employees.push_back(entity);
                        planning.employer = businessEntity;
                        planning.agenda = business.agenda;
                        planning.currentActionIndex = {};
                        // TODO unlock lockable
                        planning.lockedEntity = {};
                    }
                }
            }
        }
    }
};

UpdaterRegisterer<BusinessUpdater> registerer;

} // namespace
