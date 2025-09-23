#include "Components/ActionPlanningComponent.hpp"
#include "Components/BusinessComponent.hpp"
#include "Components/EmployableComponent.hpp"

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
        ComponentWriter<ActionPlanningComponent> plannings =
            componentAccessor->WriteComponents<ActionPlanningComponent>();
        ComponentWriter<BusinessComponent> businesses = componentAccessor->WriteComponents<BusinessComponent>();
        ComponentWriter<EmployableComponent> employables = componentAccessor->WriteComponents<EmployableComponent>();

        for (int i = 0; i < componentAccessor->Count(); i++)
        {
            if (plannings[i])
            {
                EmployableComponent& employable = *employables[i];
                if (!employable.employer)
                {
                    const Entity entity(i);
                    Entity businessEntity = FindNearestEntity(componentAccessor, entity, IsBusiness);
                    if (businessEntity != Entity::Invalid())
                    {
                        ActionPlanningComponent& planning = *plannings[i];
                        BusinessComponent& business = *businesses[businessEntity];
                        business.employees.push_back(entity);
                        employable.employer = businessEntity;
                        planning.agenda = business.agenda;
                        planning.currentActionIndex = {};
                        // TODO unlock lockable
                        planning.lockedEntity = {};
                    }
                }
            }
        }
    }

    void OnDeletedEntity(Entity entity, IEntityManager* entityManager, ComponentAccessor* componentAccessor) override
    {
        {
            const auto& employable = componentAccessor->ReadComponents<EmployableComponent>()[entity];
            if (employable && employable->employer)
            {
                auto& employer = componentAccessor->WriteComponents<BusinessComponent>()[*employable->employer];
                HATCHER_ASSERT(employer);
                auto it = std::find(employer->employees.begin(), employer->employees.end(), entity);
                HATCHER_ASSERT(it != employer->employees.end());
                employer->employees.erase(it);
            }
        }
        {
            const auto& business = componentAccessor->ReadComponents<BusinessComponent>()[entity];
            if (business)
            {
                auto employables = componentAccessor->WriteComponents<EmployableComponent>();
                auto plannings = componentAccessor->WriteComponents<ActionPlanningComponent>();
                for (Entity employe : business->employees)
                {
                    HATCHER_ASSERT(employables[employe]);
                    HATCHER_ASSERT(plannings[employe]);
                    employables[employe]->employer = {};
                    plannings[employe]->agenda = ActionPlanningComponent::EAgenda::Derp;
                    plannings[employe]->currentActionIndex = {};
                    // TODO unlock lockable
                    plannings[employe]->lockedEntity = {};
                }
            }
        }
    }
};

UpdaterRegisterer<BusinessUpdater> registerer;

} // namespace
