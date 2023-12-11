#include "pch.h"
#include "SettingsPanel.h"

#include "Project.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    void SettingsPanel::DrawContent()
    {
        constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen
                | ImGuiTreeNodeFlags_AllowItemOverlap
                | ImGuiTreeNodeFlags_Framed
                | ImGuiTreeNodeFlags_FramePadding;

        if(ImGui::TreeNodeEx("Render Settings", treeNodeFlags, "Render Settings"))
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

        if (ImGui::Button("Save"))
            Project::SaveSettings();
    }
}
