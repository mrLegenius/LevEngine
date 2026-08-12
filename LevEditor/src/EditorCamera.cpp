#include "pch.h"
#include "EditorCamera.h"

#include "EditorMath.h"
#include "Assets/MeshAsset.h"
#include "Renderer/3D/Mesh.h"
#include "Scene/Components/MeshRenderer/MeshRenderer.h"

namespace LevEngine
{
	namespace
	{
		//<--- An entity without any geometry has no size of its own, so a fixed one is used ---<<
		constexpr float k_DefaultFocusRadius = 1.0f;
		constexpr float k_MinFocusRadius = 0.1f;
		//<--- Leaves some empty space around the focused object ---<<
		constexpr float k_FocusPadding = 1.3f;

		//<--- Meshes keep their bounds in the local space, so every corner has to go through the world matrix ---<<
		bool TryExpandByEntityBounds(const Entity entity, Vector3& min, Vector3& max)
		{
			bool hasBounds = false;

			if (entity.HasComponent<MeshRendererComponent>())
			{
				const auto& meshRenderer = entity.GetComponent<MeshRendererComponent>();

				if (meshRenderer.mesh && meshRenderer.mesh->GetMesh())
				{
					const Matrix model = Editor::GetWorldMatrix(entity.GetComponent<Transform>());

					for (const auto& corner : meshRenderer.mesh->GetMesh()->GetAABBBoundingVolume().GetVertices())
					{
						const Vector3 worldCorner = Vector3::Transform(corner, model);
						min = Vector3::Min(min, worldCorner);
						max = Vector3::Max(max, worldCorner);
					}

					hasBounds = true;
				}
			}

			for (const auto child : entity.GetComponent<Transform>().GetChildren())
			{
				if (!child) continue;

				hasBounds |= TryExpandByEntityBounds(child, min, max);
			}

			return hasBounds;
		}
	}

	EditorCamera::EditorCamera(const float fov, const float nearClip, const float farClip, Vector3 initialPosition)
	{
		m_Transform.SetLocalPosition(initialPosition);
		SetPerspective(fov, nearClip, farClip);
	}

	void EditorCamera::OnUpdate(const float deltaTime)
	{
		constexpr auto rotationSpeed = 45;
		constexpr auto zoomSensitivity = 0.5f;

		const DirectX::SimpleMath::Vector2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		if (m_InitialMousePosition.x == 0 && m_InitialMousePosition.y == 0)
			m_InitialMousePosition = mouse;

		const auto delta = (mouse - m_InitialMousePosition) * rotationSpeed * deltaTime;
		m_InitialMousePosition = mouse;

		if (Input::IsMouseButtonDown(MouseButton::Right))
		{
			auto rotation = m_Transform.GetLocalRotation().ToEuler() * Math::RadToDeg;
			rotation.y -= delta.x;
			rotation.x -= delta.y;

			rotation.x = Math::Clamp(rotation.x, -89.99f, 89.999f);
			m_Transform.SetWorldRotation(Quaternion::CreateFromYawPitchRoll(rotation * Math::DegToRad));

			if (Input::IsKeyDown(KeyCode::A))
				m_Transform.MoveLeft(deltaTime * m_MoveSpeed);
			else if (Input::IsKeyDown(KeyCode::D))
				m_Transform.MoveRight(deltaTime * m_MoveSpeed);

			if (Input::IsKeyDown(KeyCode::W))
				m_Transform.MoveForward(deltaTime * m_MoveSpeed);
			else if (Input::IsKeyDown(KeyCode::S))
				m_Transform.MoveBackward(deltaTime * m_MoveSpeed);

			if (Input::IsKeyDown(KeyCode::E))
				m_Transform.MoveUp(deltaTime * m_MoveSpeed);
			else if (Input::IsKeyDown(KeyCode::Q))
				m_Transform.MoveDown(deltaTime * m_MoveSpeed);
		}

		Zoom(deltaTime * zoomSensitivity * Input::GetMouseWheelOffset());
	}

	void EditorCamera::Zoom(const float value)
	{
		m_MoveSpeed = Math::Clamp(m_MoveSpeed + value, 0.5f, 100.0f);
	}

	void EditorCamera::Focus(const Entity entity)
	{
		if (!entity || !entity.HasComponent<Transform>()) return;

		constexpr float floatMax = std::numeric_limits<float>::max();

		Vector3 min{ floatMax, floatMax, floatMax };
		Vector3 max{ -floatMax, -floatMax, -floatMax };

		const bool hasBounds = TryExpandByEntityBounds(entity, min, max);

		if (!hasBounds)
		{
			const Vector3 position = entity.GetComponent<Transform>().GetWorldPosition();
			const Vector3 extents = Vector3::One * k_DefaultFocusRadius;

			FocusOnBounds(position - extents, position + extents);
			return;
		}

		FocusOnBounds(min, max);
	}

	void EditorCamera::Focus(const Vector3 center, const float radius)
	{
		const Vector3 extents = Vector3::One * Math::Max(radius, k_MinFocusRadius);

		FocusOnBounds(center - extents, center + extents);
	}

	Ray EditorCamera::GetViewportRay(const Vector2 point, const Vector2 viewportSize) const
	{
		if (viewportSize.x <= 0.0f || viewportSize.y <= 0.0f) return Ray{};

		//<--- The clip space has its y axis pointing up and the depth going from 0 at the near plane to 1 at the far one ---<<
		const Vector2 clipPoint
		{
			point.x / viewportSize.x * 2.0f - 1.0f,
			1.0f - point.y / viewportSize.y * 2.0f
		};

		const Matrix view = m_Transform.GetModel().Invert();
		const Matrix inverseViewProjection = (view * GetProjection()).Invert();

		//<--- Going through the inverse of the whole transformation keeps the projection type out of the picture ---<<
		const Vector3 nearPoint = Vector3::Transform(Vector3{ clipPoint.x, clipPoint.y, 0.0f }, inverseViewProjection);
		const Vector3 farPoint = Vector3::Transform(Vector3{ clipPoint.x, clipPoint.y, 1.0f }, inverseViewProjection);

		Vector3 direction = farPoint - nearPoint;
		direction.Normalize();

		return Ray{ nearPoint, direction };
	}

	void EditorCamera::FocusOnBounds(const Vector3 min, const Vector3 max)
	{
		const Vector3 center = (min + max) * 0.5f;
		const Vector3 extents = (max - min) * 0.5f;

		const Vector3 forward = m_Transform.GetForwardDirection();
		const Vector3 right = m_Transform.GetRightDirection();
		const Vector3 up = m_Transform.GetUpDirection();

		//<--- How much of the box the camera actually has to fit depends on how the box is turned
		//     towards it, so the bounds are measured along the camera axes instead of the world ones ---<<
		const float halfWidth = std::abs(extents.x * right.x) + std::abs(extents.y * right.y) + std::abs(extents.z * right.z);
		const float halfHeight = std::abs(extents.x * up.x) + std::abs(extents.y * up.y) + std::abs(extents.z * up.z);
		const float halfDepth = std::abs(extents.x * forward.x) + std::abs(extents.y * forward.y) + std::abs(extents.z * forward.z);

		const float halfFovY = GetFieldOfView() * 0.5f * Math::DegToRad;
		const float halfFovX = std::atan(std::tan(halfFovY) * m_AspectRatio);

		const float distanceByWidth = Math::Max(halfWidth, k_MinFocusRadius) / std::tan(halfFovX);
		const float distanceByHeight = Math::Max(halfHeight, k_MinFocusRadius) / std::tan(halfFovY);

		//<--- The distance is measured from the near side of the box, not from its center ---<<
		const float distance = Math::Clamp(
			Math::Max(distanceByWidth, distanceByHeight) * k_FocusPadding + halfDepth,
			GetPerspectiveNear() * 2.0f, GetPerspectiveFar() * 0.5f);

		m_Transform.SetWorldPosition(center - forward * distance);
		UpdateView();
	}
}
