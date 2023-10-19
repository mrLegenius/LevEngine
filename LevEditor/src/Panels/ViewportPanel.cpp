#include "pch.h"
#include "ViewportPanel.h"

#include <imgui.h>

#include "imguizmo/ImGuizmo.h"
#include "EntitySelection.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    ImVec2 leftBottom;
    ImVec2 rightTop;

    void ViewportPanel::DrawContent()
	{
        if (!m_Focused)
            m_Camera.ResetInitialMousePosition();

        const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        const auto viewportOffset = ImGui::GetWindowPos();
        m_Bounds[0] = Vector2{ viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_Bounds[1] = Vector2{ viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        //Application::Get().GetImGuiLayer()->BlockEvents(!m_Focused && !m_Hovered);

        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        m_Size = Vector2{ viewportSize.x, viewportSize.y };

        const float left = 0.5f * (1 - viewportSize.x / m_Texture->GetWidth());
        const float bottom = 0.5f * (1 - viewportSize.y / m_Texture->GetHeight());
	    leftBottom = ImVec2{left, bottom};

        const float right = 0.5f * (1 + viewportSize.x / m_Texture->GetWidth());
        const float top = 0.5f * (1 + viewportSize.y / m_Texture->GetHeight());
	    rightTop = ImVec2{right, top};

        ImGui::Image(
            m_Texture->GetId(),
            viewportSize,
            leftBottom,
            rightTop
        );

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(EditorGUI::AssetPayload))
            {
                const wchar_t* path = (const wchar_t*)payload->Data;
                SceneManager::LoadScene(path);
            }
            ImGui::EndDragDropTarget();
        }

        DrawGizmo();
	}

    void ViewportPanel::DrawGizmo() const
    {
        //Gizmos
        const auto& entitySelection = CastRef<EntitySelection>(Selection::Current());

        if (!entitySelection) return;

        const Entity selectedEntity = entitySelection->Get();
        if (selectedEntity && Gizmo::Tool != Gizmo::ToolType::None)
        {
            ImGuizmo::SetOrthographic(false);
            ImGuizmo::SetDrawlist();

            const auto width = m_Texture->GetWidth();
            const auto height = m_Texture->GetHeight();
            const auto xOffset = width * leftBottom.x;
            const auto yOffset = height * leftBottom.y;
            
            ImGuizmo::SetRect(m_Bounds[0].x - xOffset, m_Bounds[0].y - yOffset, width, height);

            const Matrix cameraView = m_Camera.GetTransform().GetModel().Invert();
            const Matrix& cameraProjection = m_Camera.GetProjection();

            auto& tc = selectedEntity.GetComponent<Transform>();
            Matrix model = tc.GetModel();

            const bool snap = Input::IsKeyDown(KeyCode::LeftControl);
            const float snapValue = Gizmo::Tool == Gizmo::ToolType::Rotate ? 5.0f : 0.5f;

            const float snapValues[3] = { snapValue, snapValue, snapValue };
            
            if (Manipulate(&cameraView._11,
                           &cameraProjection._11,
                           static_cast<ImGuizmo::OPERATION>(Gizmo::Tool),
                           static_cast<ImGuizmo::MODE>(Gizmo::Space),
                           &model._11,
                           nullptr,
                           snap ? snapValues : nullptr))
            {
                Vector3 position, scale;
                Quaternion rotation;
                Math::DecomposeTransform(model, position, rotation, scale);

                tc.SetWorldPosition(position);
                tc.SetWorldScale(scale);
                tc.SetWorldRotation(rotation);
                tc.RecalculateModel();
            }
        }
    }
}
