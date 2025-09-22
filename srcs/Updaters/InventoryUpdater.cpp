#include "Components/InventoryComponent.hpp"
#include "Components/PositionComponent.hpp"

#include "hatcher/ComponentAccessor.hpp"
#include "hatcher/Updater.hpp"

using namespace hatcher;

namespace
{

class InventoryUpdater final : public Updater
{
    void Update(IEntityManager* entityManager, ComponentAccessor* componentAccessor) override {}

    void OnDeletedEntity(Entity entity, IEntityManager* entityManager, ComponentAccessor* componentAccessor) override
    {
        auto positionComponents = componentAccessor->WriteComponents<PositionComponent>();
        auto inventoryComponent = componentAccessor->ReadComponents<InventoryComponent>()[entity];
        if (inventoryComponent)
        {
            HATCHER_ASSERT(positionComponents[entity]);
            const glm::vec2 position = positionComponents[entity]->position;
            for (Entity itemID : inventoryComponent->storage)
            {
                positionComponents[itemID] = PositionComponent{
                    .position = position,
                    .orientation = {1.f, 0.f},
                };
            }
        }
    }
};

UpdaterRegisterer<InventoryUpdater> registerer;

} // namespace
