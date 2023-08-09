#include "pch.h"
#include "ViewportPanel.h"

#include <imgui.h>

#include "Kernel/Application.h"

namespace LevEngine::Editor
{
	void ViewportPanel::DrawContent()
	{
        const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        const auto viewportOffset = ImGui::GetWindowPos();
        m_Bounds[0] = Vector2{ viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_Bounds[1] = Vector2{ viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        Application::Get().GetImGuiLayer()->BlockEvents(!m_Focused && !m_Hovered);

        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        m_Size = Vector2{ viewportSize.x, viewportSize.y };

        ImGui::Image(
            m_Texture->GetId(),
            viewportSize,
            ImVec2(0, 0),
            ImVec2(1, 1)
        );

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSETS_BROWSER_ITEM"))
            {
                const wchar_t* path = (const wchar_t*)payload->Data;
                //OpenScene(std::filesystem::path(g_AssetsPath) / path);
            }
            ImGui::EndDragDropTarget();
        }

        //Gizmos
    //Entity selectedEntity = m_Hierarchy.GetSelectedEntity();
    //if (selectedEntity && m_GizmoType != -1)
    //{
    //    ImGuizmo::SetOrthographic(false);
    //    ImGuizmo::SetDrawlist();

    //    ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

    //    glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();
    //    const glm::mat4& cameraProjection = m_EditorCamera.GetProjection();

    //    auto& tc = selectedEntity.GetComponent<TransformComponent>();
    //    glm::mat4 model = tc.GetModel();

    //    const bool snap = Input::IsKeyDown(KeyCode::LeftControl);
    //    const float snapValue = m_GizmoType == ImGuizmo::OPERATION::ROTATE ? 5.0f : 0.5f;

    //    float snapValues[3] = { snapValue, snapValue, snapValue };


    //    ImGuizmo::Manipulate(glm::value_ptr(cameraView),
    //        glm::value_ptr(cameraProjection),
    //        static_cast<ImGuizmo::OPERATION>(m_GizmoType),
    //        ImGuizmo::LOCAL,
    //        glm::value_ptr(model),
    //        nullptr,
    //        snap ? snapValues : nullptr);

    //    if (ImGuizmo::IsUsing())
    //    {
    //        glm::vec3 position, rotation, scale;
    //        Math::DecomposeTransform(model, position, rotation, scale);

    //        tc.position = position;

    //        //Adding delta rotation to avoid Gimbal lock
    //        tc.rotation += rotation - tc.rotation;

    //        tc.scale = scale;
    //    }
    //}
	}
}
