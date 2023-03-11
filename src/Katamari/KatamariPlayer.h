#pragma once
#include "../GameObject.h"
#include "../Components/MeshRenderer.h"
#include "../Assets.h"
#include "../Input/Input.h"


class KatamariPlayer : public GameObject
{
public:
    KatamariPlayer(const Transform& cameraTransform)
        : GameObject(
            std::make_shared<MeshRenderer>(
                ShaderAssets::Unlit(),
                Mesh::CreateSphere(45),
                TextureAssets::Rock())),
        m_CameraTransform(cameraTransform)
    {
        m_SphereCollider = std::make_shared<SphereCollider>();
        m_Collider = m_SphereCollider;
    }

    void Update(float deltaTime) override
    {
        if (Input::IsKeyPressed(KeyCode::Space))
            GetRigidbody()->AddImpulse(Vector3::Up * 100);

        Vector3 movementDir = Vector3::Zero;

        if (Input::IsKeyDown(KeyCode::W))
            movementDir = m_CameraTransform.GetForwardDirection();
        if (Input::IsKeyDown(KeyCode::A))
            movementDir = -m_CameraTransform.GetRightDirection();
        if (Input::IsKeyDown(KeyCode::S))
            movementDir = -m_CameraTransform.GetForwardDirection();
        if (Input::IsKeyDown(KeyCode::D))
            movementDir = m_CameraTransform.GetRightDirection();

        movementDir.y = 0.0f;
        movementDir.Normalize();
        GetRigidbody()->AddForceAtPosition(movementDir * 100, GetTransform()->GetWorldPosition() + Vector3::Up);

        if (Input::IsKeyPressed(KeyCode::Tab))
        {
	        for (auto child : m_Transform->GetChildren())
	        {
                child->SetParent(nullptr);
	        }
        }
    }

    void OnCollisionBegin(GameObject* gameObject) override
    {
        if (gameObject->GetRigidbody()->bodyType == BodyType::Static) return;

        auto radius = gameObject->GetCollider()->GetRadius();
        if (radius > m_SphereCollider->radius) return;

		//GetRigidbody()->mass += gameObject->GetRigidbody()->mass;
        m_SphereCollider->radius += 0.1f;

        gameObject->GetRigidbody()->enabled = false;
       
        gameObject->GetTransform()->SetParent(m_Transform.get());
    }

private:
    std::shared_ptr<SphereCollider> m_SphereCollider;
    const Transform& m_CameraTransform;
};
