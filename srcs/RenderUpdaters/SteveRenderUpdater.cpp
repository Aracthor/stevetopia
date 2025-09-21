#include "RenderUpdaterOrder.hpp"

#include "hatcher/ComponentAccessor.hpp"
#include "hatcher/Graphics/IFrameRenderer.hpp"
#include "hatcher/Graphics/IRendering.hpp"
#include "hatcher/Graphics/Material.hpp"
#include "hatcher/Graphics/MaterialFactory.hpp"
#include "hatcher/Graphics/Mesh.hpp"
#include "hatcher/Graphics/MeshLoader.hpp"
#include "hatcher/Graphics/RenderUpdater.hpp"
#include "hatcher/Graphics/Texture.hpp"
#include "hatcher/Maths/glm_pure.hpp"

#include "Components/InventoryComponent.hpp"
#include "Components/ItemComponent.hpp"
#include "Components/Movement2DComponent.hpp"
#include "Components/Position2DComponent.hpp"
#include "Components/WorkerComponent.hpp"
#include "RenderComponents/ItemDisplayComponent.hpp"
#include "RenderComponents/SelectableComponent.hpp"
#include "RenderComponents/SteveAnimationComponent.hpp"
#include "utils/TransformationHelper.hpp"

using namespace hatcher;

namespace
{

class SteveAnimationUpdater final : public RenderUpdater
{
public:
    SteveAnimationUpdater(const IRendering* rendering) {}

    void Update(IApplication* application, const ComponentAccessor* componentAccessor,
                ComponentAccessor* renderComponentAccessor, IFrameRenderer& frameRenderer) override
    {
        const auto positionComponents = componentAccessor->ReadComponents<Position2DComponent>();
        const auto movementComponents = componentAccessor->ReadComponents<Movement2DComponent>();
        const auto inventoryComponents = componentAccessor->ReadComponents<InventoryComponent>();
        const auto itemComponents = componentAccessor->ReadComponents<ItemComponent>();
        const auto workerComponents = componentAccessor->ReadComponents<WorkerComponent>();
        auto itemDisplayComponents = renderComponentAccessor->WriteComponents<ItemDisplayComponent>();
        auto animationComponents = renderComponentAccessor->WriteComponents<SteveAnimationComponent>();

        for (int i = 0; i < componentAccessor->Count(); i++)
        {
            if (positionComponents[i] && movementComponents[i] && animationComponents[i])
            {
                SteveAnimationComponent& animation = *animationComponents[i];
                const bool moving = !movementComponents[i]->path.empty();
                const bool working = workerComponents[i] && workerComponents[i]->workIndex;
                UpdateAnimationComponent(animation, moving, working);

                const auto IsResource = [&itemComponents](Entity entity)
                { return itemComponents[entity]->type == ItemComponent::Resource; };
                if (inventoryComponents[i] &&
                    std::find_if(inventoryComponents[i]->storage.begin(), inventoryComponents[i]->storage.end(),
                                 IsResource) != inventoryComponents[i]->storage.end())
                {
                    animation.rightArmAngle = M_PI;
                    animation.leftArmAngle = M_PI;
                }

                HATCHER_ASSERT(itemDisplayComponents[i]);
                ItemDisplayComponent& itemDisplayComponent = *itemDisplayComponents[i];
                glm::mat4 toolLocation(1.f);
                toolLocation = glm::translate(toolLocation, glm::vec3(0.0f, -0.3f, 1.1f));
                toolLocation = glm::rotate(toolLocation, -animation.rightArmAngle + static_cast<float>(M_PI) / 2.f,
                                           glm::vec3(0.f, 1.f, 0.0f));
                toolLocation = glm::translate(toolLocation, glm::vec3(0.4f, 0.0f, -0.2f));
                itemDisplayComponent.locations[std::make_pair(ItemComponent::Tool, 0)] = toolLocation;

                glm::mat4 resourceLocation(1.f);
                resourceLocation = glm::translate(resourceLocation, glm::vec3(0.f, 0.f, 1.8f));
                itemDisplayComponent.locations[std::make_pair(ItemComponent::Resource, 0)] = resourceLocation;
            }
        }
    }

private:
    void UpdateAnimationComponent(SteveAnimationComponent& animationComponent, bool moving, bool working)
    {
        const float legMoveSpeed = 0.1f;
        const float legMaxAngle = M_PI / 4.f;
        if (moving)
        {
            const float legSign = animationComponent.rightLegRising ? -1.f : 1.f;
            animationComponent.rightLegAngle += legSign * legMoveSpeed;
            if (std::abs(animationComponent.rightLegAngle) > legMaxAngle)
            {
                const float angleToBackdown = std::abs(animationComponent.rightLegAngle) - legMaxAngle;
                animationComponent.rightLegAngle -= legSign * angleToBackdown * 2.f;
                animationComponent.rightLegRising = !animationComponent.rightLegRising;
            }
        }
        else if (animationComponent.rightLegAngle != 0.f)
        {
            const float legSign = (animationComponent.rightLegAngle > 0.f) ? -1.f : 1.f;
            animationComponent.rightLegAngle += legSign * legMoveSpeed;
            if (animationComponent.rightLegAngle * legSign > 0.f)
                animationComponent.rightLegAngle = 0.f;
        }

        const float armMoveSpeed = 0.05f;
        animationComponent.rightArmAngle = 0.f;
        animationComponent.leftArmAngle = 0.f;
        if (working)
        {
            animationComponent.rightArmProgress += armMoveSpeed;
            if (animationComponent.rightArmProgress > 1.f)
                animationComponent.rightArmProgress -= 2.f;
            animationComponent.rightArmAngle = std::acos(animationComponent.rightArmProgress);
            if (animationComponent.rightArmAngle > M_PI / 2.f)
                animationComponent.rightArmAngle = M_PI - animationComponent.rightArmAngle;
            animationComponent.rightArmAngle += M_PI / 2.f;
        }
    }
};

class SteveRenderUpdater final : public RenderUpdater
{
public:
    SteveRenderUpdater(const IRendering* rendering)
        : m_bodyParts({&m_torso, &m_head, &m_leftArm, &m_rightArm, &m_leftLeg, &m_rightLeg})
    {
        m_material = rendering->GetMaterialFactory()->CreateMaterial("shaders/textured.vert", "shaders/textured.frag");

        const Texture* texture = rendering->GetMaterialFactory()->TextureFromFile("assets/textures/skins/steve.bmp");
        m_material->AddTexture("uniTexture", texture);

        const Material* material = m_material.get();
        MeshLoader* meshLoader = rendering->GetMeshLoader().get();
        m_torso.mesh = meshLoader->LoadWavefront(material, "assets/meshes/steve/torso.obj");
        m_head.mesh = meshLoader->LoadWavefront(material, "assets/meshes/steve/head.obj");
        m_leftArm.mesh = meshLoader->LoadWavefront(material, "assets/meshes/steve/left_arm.obj");
        m_rightArm.mesh = meshLoader->LoadWavefront(material, "assets/meshes/steve/right_arm.obj");
        m_leftLeg.mesh = meshLoader->LoadWavefront(material, "assets/meshes/steve/left_leg.obj");
        m_rightLeg.mesh = meshLoader->LoadWavefront(material, "assets/meshes/steve/right_leg.obj");

        m_torso.matrix = glm::mat4(1.f);
        m_head.matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 1.2f));
        m_leftArm.matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.3f, 1.1f));
        m_rightArm.matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -0.3f, 1.1f));
        m_leftLeg.matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, 0.1f, 0.6f));
        m_rightLeg.matrix = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -0.1f, 0.6f));
    }

    ~SteveRenderUpdater() = default;

    void Update(IApplication* application, const ComponentAccessor* componentAccessor,
                ComponentAccessor* renderComponentAccessor, IFrameRenderer& frameRenderer) override
    {
        frameRenderer.PrepareSceneDraw(m_material.get());

        const auto positionComponents = componentAccessor->ReadComponents<Position2DComponent>();
        auto animationComponents = renderComponentAccessor->WriteComponents<SteveAnimationComponent>();

        for (int i = 0; i < componentAccessor->Count(); i++)
        {
            if (positionComponents[i] && animationComponents[i])
            {
                const glm::mat4 modelMatrix = TransformationHelper::ModelFromComponents(*positionComponents[i]);
                SteveAnimationComponent& animation = *animationComponents[i];
                const glm::mat4 rightLegMatrix =
                    glm::rotate(m_rightLeg.matrix, animation.rightLegAngle, glm::vec3(0.f, 1.f, 0.f));
                const glm::mat4 leftLegMatrix =
                    glm::rotate(m_leftLeg.matrix, -animation.rightLegAngle, glm::vec3(0.f, 1.f, 0.f));
                float rightArmAngle = animation.rightArmAngle;
                float leftArmAngle = animation.leftArmAngle;
                const glm::mat4 rightArmMatrix =
                    glm::rotate(m_rightArm.matrix, rightArmAngle, glm::vec3(0.f, -1.f, 0.f));
                const glm::mat4 leftArmMatrix = glm::rotate(m_leftArm.matrix, leftArmAngle, glm::vec3(0.f, 1.f, 0.f));
                m_torso.mesh->Draw(modelMatrix * m_torso.matrix);
                m_head.mesh->Draw(modelMatrix * m_head.matrix);
                m_rightArm.mesh->Draw(modelMatrix * rightArmMatrix);
                m_leftArm.mesh->Draw(modelMatrix * leftArmMatrix);
                m_rightLeg.mesh->Draw(modelMatrix * rightLegMatrix);
                m_leftLeg.mesh->Draw(modelMatrix * leftLegMatrix);
            }
        }
    }

    void OnCreateEntity(Entity entity, const ComponentAccessor* componentAccessor,
                        ComponentAccessor* renderComponentAccessor) override
    {
        const auto& animationComponent = renderComponentAccessor->ReadComponents<SteveAnimationComponent>()[entity];
        auto& selectableComponent = renderComponentAccessor->WriteComponents<SelectableComponent>()[entity];

        if (animationComponent && selectableComponent)
        {
            for (const BodyPart* bodyPart : m_bodyParts)
            {
                const Box3f box = bodyPart->mesh->Box().Translated(glm::vec3(bodyPart->matrix[3]));
                selectableComponent->box.Add(box.Scaled(1.1f));
            }
        }
    }

    struct BodyPart
    {
        unique_ptr<Mesh> mesh;
        glm::mat4 matrix;
    };

    unique_ptr<Material> m_material;
    BodyPart m_torso;
    BodyPart m_head;
    BodyPart m_leftArm;
    BodyPart m_rightArm;
    BodyPart m_leftLeg;
    BodyPart m_rightLeg;
    std::array<BodyPart*, 6> m_bodyParts;
};

RenderUpdaterRegisterer<SteveAnimationUpdater> animationRegisterer((int)ERenderUpdaterOrder::PreRender);
RenderUpdaterRegisterer<SteveRenderUpdater> registerer((int)ERenderUpdaterOrder::Scene);

} // namespace
