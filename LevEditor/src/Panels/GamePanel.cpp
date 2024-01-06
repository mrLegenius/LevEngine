#include "pch.h"
#include "GamePanel.h"

#include "SceneState.h"

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

        //Application::Get().GetImGuiLayer()->BlockEvents(!m_Focused && !m_Hovered);

        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        m_Size = Vector2{ viewportSize.x, viewportSize.y };

        const ImVec2 windowSize = GetLargestSizeForViewport();
        const ImVec2 windowPos = GetCenteredPositionForViewport(windowSize);
		ImGui::SetCursorPos(windowPos);

        const auto windowAspectRatio = windowSize.x / windowSize.y;
        const auto textureAspectRatio = static_cast<float>(m_Texture->GetWidth()) / static_cast<float>(m_Texture->GetHeight());

        auto diff = windowAspectRatio / textureAspectRatio;

		ImVec2 uv0;
		ImVec2 uv1;

		if (diff > 1.0f)
		{
			diff = 1.0f / diff;
			auto leftDiff = 0.5f - diff / 2.0f;
			auto rightDiff = 0.5f + diff / 2.0f;

			uv0 = {0, leftDiff};
			uv1 = {1, rightDiff};
		}
		else
		{
			auto leftDiff = 0.5f - diff / 2.0f;
			auto rightDiff = 0.5f + diff / 2.0f;

			uv0 = {leftDiff, 0};
			uv1 = {rightDiff, 1};
		}
		
        ImGui::Image(
            m_Texture->GetId(),
            windowSize,
            uv0,
            uv1
        );

        const auto windowCenterX = ImGui::GetWindowPos().x + ImGui::GetWindowSize().x / 2.0f;
        const auto windowCenterY = ImGui::GetWindowPos().y + ImGui::GetWindowSize().y / 2.0f;

		if (m_StateGetter() == SceneState::Play && m_Focused && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
		{
			App::Get().GetWindow().SetCursorPosition(
				static_cast<uint32_t>(windowCenterX), static_cast<uint32_t>(windowCenterY));
		}
	}

	void GamePanel::OnFocus() const
	{
		if (m_StateGetter() == SceneState::Play)
		{
			App::Get().GetWindow().DisableCursor();
		}
	}

	void GamePanel::OnLostFocus() const
	{
		App::Get().GetWindow().EnableCursor();
	}
}

