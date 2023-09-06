#include "pch.h"
#include "GamePanel.h"

namespace LevEngine::Editor
{
	ImVec2 GetLargestSizeForViewport()
	{
		const ImVec2 windowSize = ImGui::GetContentRegionAvail();

		//TODO: Make selectable
		auto aspectRatio = 16.0f / 9.0f;
		
		float aspectWidth = windowSize.x;
		float aspectHeight = aspectWidth / aspectRatio;
		if (aspectHeight > windowSize.y) {
			// We must switch to pillarbox mode
			aspectHeight = windowSize.y;
			aspectWidth = aspectHeight * aspectRatio;
		}

		return {aspectWidth, aspectHeight};
	}

	ImVec2 GetCenteredPositionForViewport(const ImVec2 aspectSize)
	{
		const ImVec2 windowSize = ImGui::GetContentRegionAvail();

		const float viewportX = (windowSize.x / 2.0f) - (aspectSize.x / 2.0f);
		const float viewportY = (windowSize.y / 2.0f) - (aspectSize.y / 2.0f);

		return {viewportX + ImGui::GetCursorPosX(), viewportY + ImGui::GetCursorPosY()};
	}
	
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

        const ImVec2 windowSize = GetLargestSizeForViewport();
        const ImVec2 windowPos = GetCenteredPositionForViewport(windowSize);
		ImGui::SetCursorPos(windowPos);
		
        ImGui::Image(
            m_Texture->GetId(),
            windowSize,
            {0, 0},
            {1, 1}
        );
	}
}

