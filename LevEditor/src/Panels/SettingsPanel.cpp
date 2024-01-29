#include "pch.h"
#include "SettingsPanel.h"

#include "Project.h"
#include "GUI/EditorGUI.h"
#include "GUI/ScopedGUIHelpers.h"
#include "Physics/PhysicsSettings.h"

namespace LevEngine::Editor
{
    void SettingsPanel::DrawContent()
    {
        constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
                | ImGuiTreeNodeFlags_AllowItemOverlap
                | ImGuiTreeNodeFlags_Framed
                | ImGuiTreeNodeFlags_FramePadding;

        if (ImGui::TreeNodeEx("Render Settings", treeNodeFlags, "Render Settings"))
        {
            if(ImGui::TreeNodeEx("Post Processing", treeNodeFlags, "Post Processing"))
            {
                EditorGUI::DrawCheckBox("Enable Bloom", RenderSettings::IsBloomEnabled);

                if (RenderSettings::IsBloomEnabled)
                {
                    EditorGUI::DrawFloatControl("Bloom Threshold", RenderSettings::BloomThreshold,
                                                0.05f, 0.0f, 20.0f);

                    EditorGUI::DrawFloatControl("Bloom Magnitude", RenderSettings::BloomMagnitude,
                                                0.05f, 0.0f, 20.0f);

                    EditorGUI::DrawFloatControl("Bloom Blur Sigma", RenderSettings::BloomBlurSigma,
                                                0.05f, 0.5f, 1.5f);
                }

                ImGui::Separator();

                EditorGUI::DrawCheckBox("Enable Eye Adaptation", RenderSettings::IsEyeAdaptationEnabled);

                if (RenderSettings::IsEyeAdaptationEnabled)
                {
                    EditorGUI::DrawFloatControl("Adaptation Rate", RenderSettings::AdaptationRate,
                                                0.05f, 0.0f, 4.0f);

                    EditorGUI::DrawFloatControl("Key Value", RenderSettings::KeyValue,
                                                0.05f, 0.0f, 0.5f);
                    
                    EditorGUI::DrawFloatControl("Min Exposure", RenderSettings::MinExposure,
                           0.05f, 0.0f, 10.0f);
                    
                    if (RenderSettings::MinExposure > RenderSettings::MaxExposure)
                        RenderSettings::MinExposure = RenderSettings::MaxExposure;
                    
                    EditorGUI::DrawFloatControl("Max Exposure", RenderSettings::MaxExposure,
                           0.05f, 0.0f, 10.0f);

                    if (RenderSettings::MaxExposure < RenderSettings::MinExposure)
                        RenderSettings::MaxExposure = RenderSettings::MinExposure;
                }
                else
                {
                    EditorGUI::DrawFloatControl("Exposure", RenderSettings::ManualExposure,
                               0.01f, 0.0f, 10.0f);
                }

                ImGui::Separator();

                EditorGUI::DrawCheckBox("Enable Vignette", RenderSettings::IsVignetteEnabled);
                
                if (RenderSettings::IsVignetteEnabled)
                {
                    EditorGUI::DrawColor3Control("Color", RenderSettings::VignetteColor);
                    EditorGUI::DrawVector2Control("Center", RenderSettings::VignetteCenter, 0.5f);
                    EditorGUI::DrawFloatControl("Radius", RenderSettings::VignetteRadius, 0.01f, 0.0f, 1.0f);
                    EditorGUI::DrawFloatControl("Softness", RenderSettings::VignetteSoftness, 0.01f, 0.0f, 1.0f);
                    EditorGUI::DrawFloatControl("Intensity", RenderSettings::VignetteIntensity, 0.01f, 0.0f, 1.0f);
                }

                ImGui::Separator();
                
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::Separator();
        
        if (ImGui::TreeNodeEx("Physics Settings", treeNodeFlags, "Physics Settings"))
        {
            if (ImGui::TreeNodeEx("Layer Collision Matrix", treeNodeFlags, "Layer Collision Matrix"))
            {
                const char* columnNames[] = {
                    "", "L 0", "L 1", "L 2", "L 3", "L 4",
                        "L 5", "L 6", "L 7", "L 8", "L 9"
                };
                
                constexpr int columnsCount = IM_ARRAYSIZE(columnNames);
                constexpr int rowsCount = columnsCount - 1;

                constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit
                                                     | ImGuiTableFlags_BordersOuter
                                                     | ImGuiTableFlags_BordersInnerH
                                                     | ImGuiTableFlags_Hideable
                                                     | ImGuiTableFlags_BordersInner
                                                     | ImGuiTableFlags_NoHostExtendX;

                if (ImGui::BeginTable("CollisionMatrix", columnsCount, tableFlags))
                {
                    ImGui::TableSetupColumn(columnNames[0], ImGuiTableColumnFlags_NoReorder | ImGuiTableColumnFlags_NoResize);
                    
                    for (int column = 1; column < columnsCount; column++)
                    {
                        ImGui::TableSetupColumn(columnNames[column], ImGuiTableColumnFlags_WidthFixed);
                    }
                    
                    ImGui::TableHeadersRow();
                    for (int row = 0; row < rowsCount; row++)
                    {
                        GUI::ScopedID rowId { row };
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::AlignTextToFramePadding();
                        ImGui::Text("L %d", row);
                        for (int column = 1; column < columnsCount; column++)
                        {
                            if (ImGui::TableSetColumnIndex(column))
                            {
                                if (row >= column) continue;

                                GUI::ScopedID columnId { column };
                                
                                const auto firstLayer = static_cast<FilterLayer>(1 << row);
                                const auto secondLayer = static_cast<FilterLayer>(1 << (column - 1));

                                auto status = PhysicsSettings::IsCollisionEnabled(firstLayer, secondLayer);
                                if (EditorGUI::DrawCheckBox("", status))
                                {
                                    PhysicsSettings::EnableCollision(firstLayer, secondLayer, status);
                                    PhysicsSettings::EnableCollision(secondLayer, firstLayer, status);
                                }
                            }
                        }
                    }
                    ImGui::EndTable();
                }
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }

        ImGui::Separator(); 

        if (ImGui::Button("Save"))
            Project::SaveSettings();
    }
}