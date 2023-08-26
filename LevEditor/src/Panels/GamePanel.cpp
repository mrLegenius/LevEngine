#include "pch.h"
#include "GamePanel.h"

namespace LevEngine::Editor
{
	void GamePanel::DrawContent()
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
	}
}

