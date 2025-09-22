#include <SDL2/SDL_events.h>

#include "hatcher/CommandRegisterer.hpp"
#include "hatcher/ComponentAccessor.hpp"
#include "hatcher/EntityDescriptorID.hpp"
#include "hatcher/EntityEgg.hpp"
#include "hatcher/Graphics/IEventListener.hpp"
#include "hatcher/Graphics/IFrameRenderer.hpp"
#include "hatcher/Graphics/IRendering.hpp"
#include "hatcher/Graphics/Material.hpp"
#include "hatcher/Graphics/MaterialFactory.hpp"
#include "hatcher/Graphics/Mesh.hpp"
#include "hatcher/Graphics/MeshLoader.hpp"
#include "hatcher/Graphics/RenderUpdater.hpp"
#include "hatcher/ICommand.hpp"
#include "hatcher/ICommandManager.hpp"
#include "hatcher/IEntityManager.hpp"
#include "hatcher/assert.hpp"

#include "RenderUpdaterOrder.hpp"

#include "Components/InventoryComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/Position2DComponent.hpp"

#include "WorldComponents/Blueprint.hpp"
#include "WorldComponents/Camera.hpp"
#include "WorldComponents/SquareGrid.hpp"

using namespace hatcher;

namespace
{
struct BlueprintDescriptor
{
    EntityDescriptorID building;
    EntityDescriptorID furniture;
    glm::vec2 furniturePosition;
    std::vector<EntityDescriptorID> items;

    void Save(DataSaver& saver) const
    {
        saver << building;
        saver << furniture;
        saver << furniturePosition;
        saver << items;
    }

    void Load(DataLoader& loader)
    {
        loader >> building;
        loader >> furniture;
        loader >> furniturePosition;
        loader >> items;
    }
};

const BlueprintDescriptor loggingHut{
    .building = EntityDescriptorID::Create("LoggingHut"),
    .furniture = EntityDescriptorID::Create("Rack"),
    .furniturePosition = glm::vec2(0.f, 2.f),
    .items =
        {
            EntityDescriptorID::Create("Axe"),
            EntityDescriptorID::Create("Axe"),
            EntityDescriptorID::Create("Axe"),
        },
};

class CreateBuildingFromBlueprint final : public ICommand
{
public:
    CreateBuildingFromBlueprint(const BlueprintDescriptor& blueprint, glm::vec2 spawnPosition)
        : m_blueprint(blueprint)
        , m_spawnPosition(spawnPosition)
    {
    }

    void Save(DataSaver& saver) const override
    {
        m_blueprint.Save(saver);
        saver << m_spawnPosition;
    }

    void Load(DataLoader& loader) override
    {
        m_blueprint.Load(loader);
        loader >> m_spawnPosition;
    }

    void Execute(IEntityManager* entityManager, ComponentAccessor* componentAccessor) override
    {
        EntityEgg buildingEgg = entityManager->CreateNewEntity(m_blueprint.building);
        buildingEgg.GetComponent<Position2DComponent>()->position = m_spawnPosition;
        EntityEgg furnitureEgg = entityManager->CreateNewEntity(m_blueprint.furniture);
        furnitureEgg.GetComponent<Position2DComponent>()->position = m_spawnPosition + m_blueprint.furniturePosition;
        furnitureEgg.GetComponent<Position2DComponent>()->orientation = glm::vec2(0.f, 1.f);
        auto& inventory = furnitureEgg.GetComponent<InventoryComponent>()->storage;
        for (const EntityDescriptorID& itemDescriptor : m_blueprint.items)
        {
            EntityEgg newItem = entityManager->CreateNewEntity(itemDescriptor);
            newItem.GetComponent<ItemComponent>()->inventory = furnitureEgg.NewEntityID();
            inventory.push_back(newItem.NewEntityID());
        }
    }

private:
    BlueprintDescriptor m_blueprint;
    glm::vec2 m_spawnPosition;

    COMMAND_HEADER(CreateBuildingFromBlueprint)
};
REGISTER_COMMAND(CreateBuildingFromBlueprint);

bool CanCreateBuilding(const SquareGrid* squareGrid, glm::vec2 position)
{
    const Box2i obstacle(glm::ivec2(-1, -1), glm::ivec2(1, 2)); // TODO read ObstacleComponent.
    for (int y = obstacle.Min().y; y <= obstacle.Max().y; y++)
    {
        for (int x = obstacle.Min().x; x <= obstacle.Max().x; x++)
        {
            const glm::vec2 tilePosition = position + glm::vec2({x, y});
            if (!squareGrid->GetTileData(tilePosition).walkable)
                return false;
        }
    }
    return true;
}

class BlueprintEventListener : public IEventListener
{
    void GetEvent(const SDL_Event& event, IApplication* application, ICommandManager* commandManager,
                  const ComponentAccessor* componentAccessor, ComponentAccessor* renderComponentAccessor,
                  const IFrameRenderer& frameRenderer) override
    {
        Blueprint* blueprint = renderComponentAccessor->WriteWorldComponent<Blueprint>();
        if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_B)
        {
            blueprint->active = !blueprint->active;
        }
        else if (event.type == SDL_MOUSEMOTION)
        {
            const Camera* camera = renderComponentAccessor->ReadWorldComponent<Camera>();
            const SquareGrid* grid = componentAccessor->ReadWorldComponent<SquareGrid>();
            const glm::vec2 worldCoords2D =
                camera->MouseCoordsToWorldCoords(event.motion.x, event.motion.y, frameRenderer);
            blueprint->position = grid->GetTileCenter(worldCoords2D);
            blueprint->possible = CanCreateBuilding(grid, blueprint->position);
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && blueprint->active &&
                 blueprint->possible)
        {
            blueprint->active = false;
            commandManager->AddCommand(new CreateBuildingFromBlueprint(loggingHut, blueprint->position));
        }
    }
};

class BlueprintRenderUpdater final : public RenderUpdater
{
public:
    BlueprintRenderUpdater(const IRendering* rendering)
    {
        m_material = rendering->GetMaterialFactory()->CreateMaterial("shaders/colored.vert", "shaders/blueprint.frag");
        m_mesh = rendering->GetMeshLoader()->LoadWavefront(m_material.get(), "assets/meshes/hut.obj");
    }

    void Update(IApplication* application, const ComponentAccessor* componentAccessor,
                ComponentAccessor* renderComponentAccessor, IFrameRenderer& frameRenderer) override
    {
        const Blueprint* blueprint = renderComponentAccessor->WriteWorldComponent<Blueprint>();
        if (blueprint->active)
        {
            constexpr glm::vec4 possibleColor = glm::vec4(0.f, 1.f, 0.f, 1.f);
            constexpr glm::vec4 impossibleColor = glm::vec4(1.f, 0.f, 0.f, 1.f);
            m_material->SetUniform("uniColor", blueprint->possible ? possibleColor : impossibleColor);
            frameRenderer.PrepareSceneDraw(m_material.get());
            const glm::mat4 modelMatrix = glm::translate(glm::mat4(1.f), glm::vec3(blueprint->position, 0.f));
            m_mesh->Draw(modelMatrix);
        }
    }

private:
    unique_ptr<Material> m_material;
    unique_ptr<Mesh> m_mesh;
};

EventListenerRegisterer<BlueprintEventListener> eventRegisterer;
RenderUpdaterRegisterer<BlueprintRenderUpdater> updateRegisterer((int)ERenderUpdaterOrder::Camera);

} // namespace