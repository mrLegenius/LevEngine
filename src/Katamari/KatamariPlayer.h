#pragma once
#include "Scene/System.h"
#include "../Input/Input.h"
#include "Scene/Components/Transform/Transform.h"
#include "Scene/Entity.h"
#include "Physics/Events/CollisionBeginEvent.h"
#include "Scene/Components/ComponentDrawer.h"
#include "Scene/Components/ComponentSerializer.h"

using namespace LevEngine;

struct KatamariPlayerComponent
{
    int s;
    KatamariPlayerComponent() = default;
    KatamariPlayerComponent(const KatamariPlayerComponent&) = default;
};

class KatamariPlayerSerializer final : public ComponentSerializer<KatamariPlayerComponent, KatamariPlayerSerializer>
{
protected:
	const char* GetKey() override { return "Katamari Player"; }

	void SerializeData(YAML::Emitter& out, const KatamariPlayerComponent& component) override
	{
        out << YAML::Key << "s" << YAML::Value << component.s;
	}

	void DeserializeData(YAML::Node& node, KatamariPlayerComponent& component) override
	{
        component.s = node["s"].as<int>();
	}
};

class KatamariPlayerDrawer final : public ComponentDrawer<KatamariPlayerComponent, KatamariPlayerDrawer>
{
protected:
	std::string GetLabel() const override { return "Katamari Player"; }

	void DrawContent(KatamariPlayerComponent& component) override
	{
	}
};

class KatamariPlayerSystem : public System
{
public:
    void Update(float deltaTime, entt::registry& registry) override
    {
        auto mainCameraEntity = registry.ctx().find<Entity>("mainCameraEntity"_hs);

        if (!mainCameraEntity) return;

        auto view = registry.view<KatamariPlayerComponent, Transform, Rigidbody>();
        for(auto entity : view)
        {
            auto [transform, rigidbody] = registry.get<Transform, Rigidbody>(entity);
            
            auto& cameraTransform = mainCameraEntity->GetComponent<Transform>();
            if (Input::IsKeyPressed(KeyCode::Space))
                rigidbody.AddImpulse(Vector3::Up * 100);

            Vector3 movementDir = Vector3::Zero;

            if (Input::IsKeyDown(KeyCode::W))
                movementDir = cameraTransform.GetForwardDirection();
            if (Input::IsKeyDown(KeyCode::A))
                movementDir = -cameraTransform.GetRightDirection();
            if (Input::IsKeyDown(KeyCode::S))
                movementDir = -cameraTransform.GetForwardDirection();
            if (Input::IsKeyDown(KeyCode::D))
                movementDir = cameraTransform.GetRightDirection();

            movementDir.y = 0.0f;
            movementDir.Normalize();
            rigidbody.AddForceAtPosition(movementDir * 100, Vector3::Up);
        }
    }
};

class KatamariCollisionSystem : public System
{
public:
    void Update(float deltaTime, entt::registry& registry) override
    {
	    const auto view = registry.view<KatamariPlayerComponent, CollisionBeginEvent, Rigidbody, SphereCollider>();
        int count = 0;
        for (const auto entity : view)
        {
            count++;
            auto [transform, collision, rigidbody, collider] = registry.get<Transform, CollisionBeginEvent, Rigidbody, SphereCollider>(entity);

            auto& otherRigidbody = collision.other.GetComponent<Rigidbody>();
            auto& otherTransform = collision.other.GetComponent<Transform>();
            if (otherRigidbody.bodyType == BodyType::Static) return;

            const auto size = collider.radius;
            const auto otherSize = Math::MaxElement(otherTransform.GetWorldScale());
            
            if (size <= otherSize) return;

            collider.radius += otherSize;

            otherRigidbody.enabled = false;
            
            otherTransform.SetParent(Entity{ entt::handle{registry, entity} });
        }
    }
};
