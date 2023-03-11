#pragma once
#include "Components/RendererComponent.h"
#include "DirectXCollision.h"
#include "Rigidbody.h"
#include "Components/Collider.h"
#include "Components/Constraint.h"

class GameObject
{
public:
	explicit GameObject(const std::shared_ptr<RendererComponent>& renderer)
		: m_Transform(std::make_shared<Transform>()),
		m_Renderer(renderer),
		m_Rigidbody(std::make_shared<Rigidbody>(m_Transform))
	{
		m_BoundingBox = std::make_shared<DirectX::BoundingBox>(m_DefaultBoundingBox.Center, m_DefaultBoundingBox.Extents);
	}

	explicit GameObject(const std::shared_ptr<RendererComponent>& renderer, const std::shared_ptr<Collider>& collider)
		: m_Transform(std::make_shared<Transform>()),
		m_Renderer(renderer),
		m_Rigidbody(std::make_shared<Rigidbody>(m_Transform)),
		m_Collider(collider)
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
	std::shared_ptr<Rigidbody>& GetRigidbody() { return m_Rigidbody; }
	std::shared_ptr<Collider>& GetCollider() { return m_Collider; }
	std::vector<std::shared_ptr<Constraint>>& GetConstraints() { return m_Constraints; }
	void AddConstraint(std::shared_ptr<Constraint> constraint) { m_Constraints.emplace_back(constraint); }

	bool Intersects(const GameObject& go) const { return m_BoundingBox->Intersects(*go.m_BoundingBox); }

	virtual void Update(float deltaTime)
	{
		m_DefaultBoundingBox.Transform(*m_BoundingBox, m_Transform->GetModel());
	}

	void Draw() const
	{
		m_Renderer->Draw(m_Transform);
	}

	virtual void OnCollisionBegin(GameObject* gameObject) { }
	virtual void OnCollisionEnd(GameObject* gameObject) { }


protected:

	DirectX::BoundingBox m_DefaultBoundingBox = DirectX::BoundingBox(Vector3::Zero, DirectX::SimpleMath::Vector3::One / 2.0f);

	std::shared_ptr<Transform> m_Transform;
	std::shared_ptr<RendererComponent> m_Renderer;
	std::shared_ptr<DirectX::BoundingBox> m_BoundingBox;
	std::shared_ptr<Rigidbody> m_Rigidbody;
	std::shared_ptr<Collider> m_Collider;
	std::vector<std::shared_ptr<Constraint>> m_Constraints;
};