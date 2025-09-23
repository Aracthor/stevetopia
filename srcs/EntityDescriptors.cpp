#include "hatcher/ComponentRegisterer.hpp"
#include "hatcher/EntityDescriptor.hpp"

#include "Components/ActionPlanningComponent.hpp"
#include "Components/BusinessComponent.hpp"
#include "Components/EmployableComponent.hpp"
#include "Components/GrowableComponent.hpp"
#include "Components/HarvestableComponent.hpp"
#include "Components/InventoryComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/LockableComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/NameComponent.hpp"
#include "Components/ObstacleComponent.hpp"
#include "Components/PositionComponent.hpp"
#include "Components/WorkerComponent.hpp"
#include "RenderComponents/ItemDisplayComponent.hpp"
#include "RenderComponents/SelectableComponent.hpp"
#include "RenderComponents/StaticMeshComponent.hpp"
#include "RenderComponents/SteveAnimationComponent.hpp"
#include "utils/TimeOfDay.hpp"

using namespace hatcher;

namespace
{

ComponentTypeRegisterer<ActionPlanningComponent, EComponentList::Gameplay> actionPlanningRegisterer;
ComponentTypeRegisterer<BusinessComponent, EComponentList::Gameplay> businessRegisterer;
ComponentTypeRegisterer<EmployableComponent, EComponentList::Gameplay> employableRegisterer;
ComponentTypeRegisterer<GrowableComponent, EComponentList::Gameplay> growableRegisterer;
ComponentTypeRegisterer<InventoryComponent, EComponentList::Gameplay> inventoryRegisterer;
ComponentTypeRegisterer<HarvestableComponent, EComponentList::Gameplay> harvestableRegisterer;
ComponentTypeRegisterer<ItemComponent, EComponentList::Gameplay> itemRegisterer;
ComponentTypeRegisterer<LockableComponent, EComponentList::Gameplay> lockableRegisterer;
ComponentTypeRegisterer<MovementComponent, EComponentList::Gameplay> movement2DRegisterer;
ComponentTypeRegisterer<NameComponent, EComponentList::Gameplay> nameRegisterer;
ComponentTypeRegisterer<ObstacleComponent, EComponentList::Gameplay> obstacleRegisterer;
ComponentTypeRegisterer<PositionComponent, EComponentList::Gameplay> position2DRegisterer;
ComponentTypeRegisterer<WorkerComponent, EComponentList::Gameplay> workerRegisterer;

ComponentTypeRegisterer<ItemDisplayComponent, EComponentList::Rendering> itemDisplayRegisterer;
ComponentTypeRegisterer<SelectableComponent, EComponentList::Rendering> selectableRegisterer;
ComponentTypeRegisterer<StaticMeshComponent, EComponentList::Rendering> staticMeshRegisterer;
ComponentTypeRegisterer<SteveAnimationComponent, EComponentList::Rendering> steveAnimationRegisterer;

EntityDescriptorRegisterer Axe{
    EntityDescriptorID::Create("Axe"),
    {
        ItemComponent{
            .type = ItemComponent::Tool,
        },
        LockableComponent{},
        NameComponent{
            .name = "Axe",
        },
    },
    {
        StaticMeshComponent{
            .type = StaticMeshComponent::Axe,
        },
    },
};

EntityDescriptorRegisterer LoggingHut{
    EntityDescriptorID::Create("LoggingHut"),
    {
        BusinessComponent{
            .storagePosition = {1.f, 2.f},
            .agenda = ActionPlanningComponent::EAgenda::Lumberjack,
        },
        NameComponent{
            .name = "Logging Hut",
        },
        ObstacleComponent{
            .area = Box2i(glm::ivec2(-1, -1), glm::ivec2(1, 1)),
        },
        PositionComponent{},
    },
    {
        SelectableComponent{},
        StaticMeshComponent{
            .type = StaticMeshComponent::Hut,
        },
    },
};

EntityDescriptorRegisterer Melon{
    EntityDescriptorID::Create("Melon"),
    {
        NameComponent{
            .name = "Melon",
        },
        GrowableComponent{
            .maturity = 0.25,
            .growthTime = HoursToTicks(1.f),
        },
        PositionComponent{},
    },
    {
        StaticMeshComponent{
            .type = StaticMeshComponent::Melon,
        },
    },
};

// Would be constexpr if the glm was...
ItemDisplayComponent::Locations RackToolLocations()
{
    ItemDisplayComponent::Locations rackItemLocations(10, &ItemDisplayComponent::LocationHash);
    glm::mat4 rackToolLocation(1.f);
    rackToolLocation = glm::translate(rackToolLocation, glm::vec3(0.08f, 0.3f, 0.f));
    rackToolLocation = glm::rotate(rackToolLocation, glm::radians(-10.f), glm::vec3(0.f, 1.f, 0.f));
    rackItemLocations[std::make_pair(ItemComponent::Tool, 0)] = rackToolLocation;
    rackToolLocation = glm::translate(rackToolLocation, glm::vec3(0.f, -0.2f, 0.f));
    rackItemLocations[std::make_pair(ItemComponent::Tool, 1)] = rackToolLocation;
    rackToolLocation = glm::translate(rackToolLocation, glm::vec3(0.f, -0.2f, 0.f));
    rackItemLocations[std::make_pair(ItemComponent::Tool, 2)] = rackToolLocation;
    rackToolLocation = glm::translate(rackToolLocation, glm::vec3(0.f, -0.2f, 0.f));
    rackItemLocations[std::make_pair(ItemComponent::Tool, 3)] = rackToolLocation;
    return rackItemLocations;
}

EntityDescriptorRegisterer Rack{
    EntityDescriptorID::Create("Rack"),
    {
        InventoryComponent{},
        ObstacleComponent{
            .area = Box2i(glm::ivec2(0, 0)),
        },
        PositionComponent{},
    },
    {
        ItemDisplayComponent{.locations = RackToolLocations()},
        SelectableComponent{},
        StaticMeshComponent{
            .type = StaticMeshComponent::Rack,
        },
    },
};

EntityDescriptorRegisterer Steve{
    EntityDescriptorID::Create("Steve"),
    {
        ActionPlanningComponent{},
        EmployableComponent{},
        InventoryComponent{},
        MovementComponent{},
        NameComponent{
            .name = "Steve",
        },
        PositionComponent{},
        WorkerComponent{},
    },
    {
        ItemDisplayComponent{},
        SelectableComponent{},
        SteveAnimationComponent{},
    },
};

EntityDescriptorRegisterer Tree{
    EntityDescriptorID::Create("Tree"),
    {
        NameComponent{
            .name = "Tree",
        },
        GrowableComponent{
            .maturity = 0.25,
            .growthTime = HoursToTicks(1.f),
        },
        HarvestableComponent{
            .harvest = EntityDescriptorID::Create("Wood"),
            .amount = 2,
        },
        LockableComponent{},
        ObstacleComponent{
            .area = Box2i(glm::ivec2(0, 0)),
        },
        PositionComponent{},
    },
    {
        StaticMeshComponent{
            .type = StaticMeshComponent::Tree,
        },
    },
};

EntityDescriptorRegisterer Wood{
    EntityDescriptorID::Create("Wood"),
    {
        ItemComponent{
            .type = ItemComponent::Resource,
        },
        EmployableComponent{},
        LockableComponent{},
        NameComponent{
            .name = "Wood",
        },
    },
    {
        StaticMeshComponent{
            .type = StaticMeshComponent::Wood,
        },
    },
};

} // namespace
