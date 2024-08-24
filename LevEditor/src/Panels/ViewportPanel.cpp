#include "pch.h"
#include "ViewportPanel.h"

#include <imgui.h>

#include "imguizmo/ImGuizmo.h"
#include "EntitySelection.h"
#include "Assets/SceneAsset.h"
#include "GUI/EditorGUI.h"
#include "GUI/ScopedGUIHelpers.h"
#include "GUI/Icons/IconsFontAwesome6.h"

namespace LevEngine::Editor
{
    ImVec2 leftBottom;
    ImVec2 rightTop;

    ViewportPanel::ViewportPanel(): Panel()
    {
        m_WindowPadding = Vector2{0, 0};
        m_CanScroll = false;
    }

    ViewportPanel::ViewportPanel(const Ref<Texture>& renderTexture): ViewportPanel()
    {
        m_Texture = renderTexture->Clone();

        m_Size.x = renderTexture->GetWidth();
        m_Size.y = renderTexture->GetHeight();

        m_Camera.SetViewportSize(static_cast<uint32_t>(m_Size.x), static_cast<uint32_t>(m_Size.y));
    }

    void ViewportPanel::UpdateCamera(const float deltaTime)
    {
        if (m_Focused)
            m_Camera.OnUpdate(deltaTime);
        m_Camera.UpdateView();
    }

    bool ViewportPanel::OnKeyPressed(KeyPressedEvent& event)
    {
        //Shortcuts
        if (event.GetRepeatCount() > 0)
            return false;

        //Controlling editor camera
        if (Input::IsMouseButtonDown(MouseButton::Right)) return false;

        switch (event.GetKeyCode())
        {
        case KeyCode::Q:
            {
                Gizmo::Tool = Gizmo::ToolType::None;
                break;
            }
        case KeyCode::W:
            {
                Gizmo::Tool = Gizmo::ToolType::Translate;
                break;
            }
        case KeyCode::E:
            {
                Gizmo::Tool = Gizmo::ToolType::Rotate;
                break;
            }
        case KeyCode::R:
            {
                Gizmo::Tool = Gizmo::ToolType::Scale;
                break;
            }
        default:
            break;
        }

        return false;
    }

    void ViewportPanel::UpdateTexture(const Ref<Texture>& renderTexture)
    {
        const auto targetWidth = renderTexture->GetWidth();
        const auto targetHeight = renderTexture->GetHeight();

        if (targetWidth != m_Texture->GetWidth() || targetHeight != m_Texture->GetHeight())
        {
            m_Texture->Resize(targetWidth, targetHeight);
            m_Camera.SetViewportSize(targetWidth, targetHeight);
        }

        m_Texture->CopyFrom(renderTexture);
    }

    void ViewportPanel::DrawContent()
    {
        DrawToolbar();

        if (!m_Focused)
            m_Camera.ResetInitialMousePosition();

        const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        const auto viewportOffset = ImGui::GetWindowPos();
        m_Bounds[0] = Vector2{viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y};
        m_Bounds[1] = Vector2{viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y};

        //Application::Get().GetImGuiLayer()->BlockEvents(!m_Focused && !m_Hovered);

        const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

        m_Size = Vector2{viewportSize.x, viewportSize.y};

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
                const Path assetPath = static_cast<const wchar_t*>(payload->Data);

                if (const auto& scene = AssetDatabase::GetAsset<SceneAsset>(assetPath))
                {
                    scene->Load();
                }
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

            const float snapValues[3] = {snapValue, snapValue, snapValue};

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

    void ViewportPanel::DrawToolbar()
    {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float textHeight = ImGui::GetFrameHeight();
        constexpr int toolbarItems = 6;
        constexpr int toolbarSeparators = 1;
        constexpr Vector2 padding{ 5.0f, 5.0f };

        const Vector2 itemSize{textHeight * 1.5f, textHeight * 1.5f };
        const auto toolbarItemSize = ImVec2{itemSize.x, itemSize.y };
        const ImVec2 toolbarPos = {
            ImGui::GetWindowPos().x + ImGui::GetCursorPos().x + style.FramePadding.x,
            ImGui::GetWindowPos().y + ImGui::GetCursorPos().y + style.FramePadding.y };
        const ImVec2 toolbarSize = {
            toolbarItemSize.x + padding.x * 2.0f,
            toolbarItemSize.y * toolbarItems + padding.y * 2.0f + toolbarSeparators * style.ItemSpacing.y
        };

        ImGui::SetNextWindowPos(toolbarPos);
        ImGui::SetNextWindowSize(toolbarSize);

        constexpr ImGuiWindowFlags toolbarFlags =
            ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoScrollWithMouse
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoBringToFrontOnFocus;

        constexpr ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_NoPadWithHalfSpacing;

        GUI::ScopedVariable windowPadding(ImGuiStyleVar_WindowPadding, padding);

        if (ImGui::Begin("##ViewportToolbar", nullptr, toolbarFlags))
        {
            // Bring the toolbar window always on top.
            if (ImGui::IsWindowAppearing())
                ImGui::BringWindowToDisplayFront(ImGui::GetCurrentWindow());

            GUI::ScopedVariable textAlign(ImGuiStyleVar_SelectableTextAlign, Vector2::One * 0.5f);

            {
                GUI::ScopedVariable itemSpacing(ImGuiStyleVar_ItemSpacing, Vector2::Zero);

                EditorGUI::DrawSelectable(ICON_FA_ARROW_POINTER, selectableFlags, itemSize, Gizmo::ToolType::None, Gizmo::Tool);
                EditorGUI::DrawSelectable(ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT, selectableFlags, itemSize, Gizmo::ToolType::Translate, Gizmo::Tool);
                EditorGUI::DrawSelectable(ICON_FA_ARROWS_ROTATE, selectableFlags, itemSize, Gizmo::ToolType::Rotate, Gizmo::Tool);
                EditorGUI::DrawSelectable(ICON_FA_ARROWS_TO_DOT, selectableFlags, itemSize, Gizmo::ToolType::Scale, Gizmo::Tool);
            }

            ImGui::Separator();

            {
                GUI::ScopedVariable itemSpacing(ImGuiStyleVar_ItemSpacing, Vector2::Zero);

                EditorGUI::DrawSelectable(ICON_FA_GLOBE, selectableFlags, itemSize, Gizmo::ToolSpace::World, Gizmo::Space);
                EditorGUI::DrawSelectable(ICON_FA_CUBE, selectableFlags, itemSize, Gizmo::ToolSpace::Local, Gizmo::Space);
            }

        }
        ImGui::End();
    }
}
