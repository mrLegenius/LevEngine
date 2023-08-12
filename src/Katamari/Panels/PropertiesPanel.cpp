﻿#include "pch.h"
#include "PropertiesPanel.h"

#include <imgui.h>

#include "Scene/Entity.h"
#include "../ComponentDrawers/CameraComponentDrawer.h"
#include "../ComponentDrawers/TransformDrawer.h"
#include "../ComponentDrawers/LightsDrawer.h"
#include "../ComponentDrawers/EmitterDrawer.h"
#include "../ComponentDrawers/MeshRendererDrawer.h"

namespace LevEngine::Editor
{
	PropertiesPanel::PropertiesPanel(const Ref<EntitySelection>& entitySelection) : m_Selection(entitySelection)
	{
		m_ComponentDrawers = {
			CreateRef<TransformDrawer>(),
			CreateRef<CameraComponentDrawer>(),
			CreateRef<DirectionalLightDrawer>(),
			CreateRef<PointLightDrawer>(),
			CreateRef<EmitterDrawer>(),
			CreateRef<MeshRendererDrawer>(),
		};
	}

	void PropertiesPanel::DrawComponents(Entity entity) const
	{
	if (entity.HasComponent<TagComponent>())
	{
		auto& tag = entity.GetComponent<TagComponent>().tag;

		char buffer[256] = {};
		strcpy_s(buffer, sizeof buffer, tag.c_str());

		if (ImGui::InputText("##Tag", buffer, sizeof buffer))
		{
			tag = std::string(buffer);
		}
	}

	ImGui::SameLine();
	ImGui::PushItemWidth(-1);

	if (ImGui::Button("Add Component"))
		ImGui::OpenPopup("AddComponent");

	if (ImGui::BeginPopup("AddComponent"))
	{
		if (m_Selection->Get())
		{
			for (const Ref<IComponentDrawer>& componentDrawer : m_ComponentDrawers)
				componentDrawer->DrawAddComponent(entity);
		}

		ImGui::EndPopup();
	}

	ImGui::PopItemWidth();

	for (const Ref<IComponentDrawer>& componentDrawer : m_ComponentDrawers)
		componentDrawer->Draw(entity);

	/*DrawComponent<MeshRendererComponent>("Mesh Renderer", entity, [](auto& component)
	{
		ImGui::Button("Shader", ImVec2(100.0f, 0.0f));
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
			{
				auto path = (const wchar_t*)payload->Data;
				std::filesystem::path shaderPath = std::filesystem::path(g_AssetsPath) / path;
				Ref<Shader> shader = Shader::Create(shaderPath.string());

				component.Shader = shader;
			}
			ImGui::EndDragDropTarget();
		}

		DrawTexture2D("Texture", [&component](const Ref<Texture2D>& texture)
		{
			component.Texture = texture;
		});

		if (ImGui::Button("Plane", ImVec2(100.0f, 0.0f)))
		{
			component.Mesh = Mesh::CreatePlane(2);
		}

		static int sphereResolution = 3;
		ImGui::DragInt("Sphere resolution", &sphereResolution, 1, 3, 100);
		if (ImGui::Button("Sphere", ImVec2(100.0f, 0.0f)))
		{
			component.Mesh = Mesh::CreateSphere(sphereResolution);
		}

		if (ImGui::Button("Cube", ImVec2(100.0f, 0.0f)))
		{
			component.Mesh = Mesh::CreateCube();
		}
	});*/
}

void PropertiesPanel::DrawContent()
{
	if (m_Selection->Get())
		DrawComponents(m_Selection->Get());
}
}
