#pragma once
#include <memory>

#include "CircularMovement.h"
#include "../Renderer/3D/Mesh.h"
#include "../Renderer/D3D11Texture.h"
#include "../Components/MeshRenderer.h"
#include "../Pong/GameObject.h"

struct BodyParameters
{
    float radius;
    float orbitRadius;
    float orbitSpeed;
    float orbitOffset;
    float axisRotationSpeed;
    std::string textureFilepath;
};

class Body : public GameObject
{
public:
    explicit Body(const std::shared_ptr<Body>& pivotBody,
        const BodyParameters& parameters,
        const std::shared_ptr<D3D11Shader>& shader)
        : GameObject(),
        m_PivotBody(pivotBody),
        m_Mesh(Mesh::CreateSphere(45)),
        m_Movement(std::make_shared<CircularMovement>()),
        m_Texture(std::make_shared<D3D11Texture2D>(parameters.textureFilepath)),
        m_AxisRotationSpeed(parameters.axisRotationSpeed)
    {
        m_Renderer = std::make_shared<MeshRenderer>(shader, m_Mesh, m_Texture);
        m_Transform->SetScale(DirectX::SimpleMath::Vector3{ parameters.radius, parameters.radius, parameters.radius });
        if (pivotBody)
            m_Movement->centerPoint = pivotBody->GetTransform();

        m_Movement->radius = parameters.orbitRadius;
        m_Movement->speed = parameters.orbitSpeed;
        m_Movement->initialOffset = parameters.orbitOffset;
        m_Movement->transform = m_Transform;
    }

    void Update(const float deltaTime) override
    {
        m_Movement->Update(deltaTime);

        GameObject::Update(deltaTime);
        auto rotation = m_Transform->GetRotation();
        rotation.y += m_AxisRotationSpeed * deltaTime;
        m_Transform->SetRotation(rotation);
    }

    std::shared_ptr<Mesh>& GetMesh() { return m_Mesh; }
    std::shared_ptr<D3D11Texture2D>& GetTexture() { return m_Texture; }

private:
    std::shared_ptr<Body> m_PivotBody;
    std::shared_ptr<Mesh> m_Mesh;
    std::shared_ptr<CircularMovement> m_Movement;
    std::shared_ptr<D3D11Texture2D> m_Texture;
    float m_AxisRotationSpeed;
};
