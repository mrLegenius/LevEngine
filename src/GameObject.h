#pragma once
#include "Components/RendererComponent.h"
#include "DirectXCollision.h"

class GameObject
{
public:
	explicit GameObject(const std::shared_ptr<RendererComponent>& renderer)
		: m_Transform(std::make_shared<Transform>()),
		m_Renderer(renderer)
	{
		m_BoundingBox = std::make_shared<DirectX::BoundingBox>(m_DefaultBoundingBox.Center, m_DefaultBoundingBox.Extents);
	}

	explicit GameObject() : m_Transform(std::make_shared<Transform>())
	{
		m_BoundingBox = std::make_shared<DirectX::BoundingBox>(m_DefaultBoundingBox.Center, m_DefaultBoundingBox.Extents);
	}

	virtual ~GameObject() = default;
	std::shared_ptr<Transform>& GetTransform() { return m_Transform; }
	std::shared_ptr<RendererComponent>& GetRenderer() { return m_Renderer; }
	bool Intersects(const GameObject& go) const { return m_BoundingBox->Intersects(*go.m_BoundingBox); }

	virtual void Update(float deltaTime)
	{
		m_DefaultBoundingBox.Transform(*m_BoundingBox, m_Transform->GetModel());
	}

	void Draw() const
	{
		m_Renderer->Draw(m_Transform);
	}

protected:

	DirectX::BoundingBox m_DefaultBoundingBox = DirectX::BoundingBox(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::One / 2.0f);

	std::shared_ptr<Transform> m_Transform;
	std::shared_ptr<RendererComponent> m_Renderer;
	std::shared_ptr<DirectX::BoundingBox> m_BoundingBox;
};