#include "pch.h"

#include "AssetDrawer.h"
#include "Assets/MaterialCustomAsset.h"
#include "Assets/ShaderAsset.h"
#include "Assets/TextureAsset.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    // Rows come from the shader, not from this file: a material knows nothing about its
    // properties beyond what the compiled shader reflected. See ShaderMaterialLayout.
    class MaterialCustomAssetDrawer final : public AssetDrawer<MaterialCustomAsset, MaterialCustomAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Shader Material"; }

        void DrawContent(Ref<MaterialCustomAsset> assetRef) override
        {
            auto shaderAsset = assetRef->GetShaderAsset();
            if (EditorGUI::DrawAsset<ShaderAsset>("Shader", shaderAsset))
                assetRef->SetShaderAsset(shaderAsset);

            auto& material = assetRef->GetCustomMaterial();

            bool isTransparent = material.IsTransparent();
            if (EditorGUI::DrawCheckBox("Transparent", isTransparent))
                material.SetTransparent(isTransparent);

            ImGui::Separator();

            if (!shaderAsset)
            {
                ImGui::TextWrapped("Assign a shader to give this material its properties.");
                return;
            }

            const auto& layout = material.GetLayout();
            if (layout.IsEmpty())
            {
                ImGui::TextWrapped(
                    "The shader declares no material properties, or it failed to compile. Check the console.");
                return;
            }

            for (const auto& property : layout.Properties)
                DrawProperty(material, property);

            for (const auto& texture : layout.Textures)
            {
                GUI::ScopedID id{texture.Name};

                auto textureAsset = assetRef->GetTexture(texture.Name);
                if (EditorGUI::DrawTextureAsset(texture.Name, textureAsset))
                    assetRef->SetTexture(texture.Name, textureAsset);
            }
        }

    private:
        // Nothing in the reflection says a float3 is a color, so the name decides which widget
        // a property gets.
        static bool IsColor(const String& name)
        {
            String lowered = name;
            lowered.make_lower();

            return lowered.find("color") != String::npos
                || lowered.find("colour") != String::npos
                || lowered.find("tint") != String::npos
                || lowered.find("albedo") != String::npos
                || lowered.find("emissive") != String::npos;
        }

        static void DrawProperty(MaterialCustom& material, const ShaderProperty& property)
        {
            GUI::ScopedID id{property.Name};

            const auto& name = property.Name;

            switch (property.Type)
            {
            case ShaderPropertyType::Float:
                EditorGUI::DrawFloatControl(name,
                                            [&material, name] { return material.GetFloat(name); },
                                            [&material, name](const float value) { material.SetFloat(name, value); },
                                            0.01f);
                break;

            case ShaderPropertyType::Float2:
                EditorGUI::DrawVector2Control(name,
                                              [&material, name] { return material.GetVector2(name); },
                                              [&material, name](const Vector2 value)
                                              {
                                                  material.SetVector2(name, value);
                                              },
                                              0.01f);
                break;

            case ShaderPropertyType::Float3:
                if (IsColor(name))
                {
                    EditorGUI::DrawColor3Control(name,
                                                 [&material, name] { return material.GetColor(name); },
                                                 [&material, name](const Color value)
                                                 {
                                                     material.SetColor(name, value);
                                                 });
                }
                else
                {
                    EditorGUI::DrawVector3Control(name,
                                                  [&material, name] { return material.GetVector3(name); },
                                                  [&material, name](const Vector3 value)
                                                  {
                                                      material.SetVector3(name, value);
                                                  },
                                                  0.01f);
                }
                break;

            case ShaderPropertyType::Float4:
                {
                    //<--- There is no float4 control, so this one talks to ImGui directly ---<<
                    if (IsColor(name))
                    {
                        Color color = material.GetColor(name);
                        if (ImGui::ColorEdit4(name.c_str(), color.Raw()))
                            material.SetColor(name, color);
                    }
                    else
                    {
                        Vector4 value = material.GetVector4(name);
                        if (ImGui::DragFloat4(name.c_str(), &value.x, 0.01f))
                            material.SetVector4(name, value);
                    }
                }
                break;

            case ShaderPropertyType::Int:
                EditorGUI::DrawIntControl(name,
                                          [&material, name] { return material.GetInt(name); },
                                          [&material, name](const int value) { material.SetInt(name, value); });
                break;

            case ShaderPropertyType::Bool:
                {
                    bool value = material.GetBool(name);
                    if (EditorGUI::DrawCheckBox(name.c_str(), value))
                        material.SetBool(name, value);
                }
                break;

            default:
                break;
            }
        }
    };
}
