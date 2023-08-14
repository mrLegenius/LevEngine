#include "pch.h"
#include "EntitySelection.h"

#include <imgui.h>

#include "ComponentDrawers/TransformDrawer.h"
#include "ComponentDrawers/CameraComponentDrawer.h"
#include "ComponentDrawers/EmitterDrawer.h"
#include "ComponentDrawers/LightsDrawer.h"
#include "ComponentDrawers/MeshRendererDrawer.h"
#include "ComponentDrawers/SkyboxComponentDrawer.h"

namespace LevEngine::Editor
{
	void EntitySelection::DrawProperties()
	{
		if (m_Entity.HasComponent<TagComponent>())
		{
			auto& tag = m_Entity.GetComponent<TagComponent>().tag;

			char buffer[256] = {};
			strcpy_s(buffer, sizeof buffer, tag.c_str());

			if (ImGui::InputText("##Tag", buffer, sizeof buffer))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		//TODO: Find a way to define them implicitly
		static std::vector<Ref<IComponentDrawer>> componentDrawers =
		{
			CreateRef<TransformDrawer>(),
			CreateRef<CameraComponentDrawer>(),
			CreateRef<DirectionalLightDrawer>(),
			CreateRef<PointLightDrawer>(),
			CreateRef<EmitterDrawer>(),
			CreateRef<MeshRendererDrawer>(),
			CreateRef<SkyboxComponentDrawer>(),
		};

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");

		if (ImGui::BeginPopup("AddComponent"))
		{
			for (const Ref<IComponentDrawer>& componentDrawer : componentDrawers)
				componentDrawer->DrawAddComponent(m_Entity);

			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		for (const Ref<IComponentDrawer>& componentDrawer : componentDrawers)
			componentDrawer->Draw(m_Entity);
	}
}
