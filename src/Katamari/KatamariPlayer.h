#pragma once
#include "Scene/System.h"
#include "../Assets.h"
#include "../Input/Input.h"
#include "Scene/Components/Components.h"
#include "Scene/Entity.h"

struct KatamariPlayerComponent
{
    int s;
    KatamariPlayerComponent() = default;
    KatamariPlayerComponent(const KatamariPlayerComponent&) = default;
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

  //  void OnCollisionBegin(GameObject* gameObject) override
  //  {
  //      if (gameObject->GetRigidbody()->bodyType == BodyType::Static) return;

  //      auto radius = gameObject->GetCollider()->GetRadius();
  //      if (radius > m_SphereCollider->radius) return;

		////GetRigidbody()->mass += gameObject->GetRigidbody()->mass;
  //      m_SphereCollider->radius += 0.1f;

  //      gameObject->GetRigidbody()->enabled = false;
  //     
  //      gameObject->GetTransform()->SetParent(m_Transform.get());
  //  }

};
