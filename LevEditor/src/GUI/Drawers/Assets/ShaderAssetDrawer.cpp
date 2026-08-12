#include "pch.h"

#include "AssetDrawer.h"
#include "Assets/ShaderAsset.h"
#include "GUI/EditorGUI.h"
#include "Renderer/Shader/Shader.h"

namespace LevEngine::Editor
{
    // Read-only view of what a shader offers a material: whether it compiled at all, and the
    // properties and textures materials made from it will show.
    class ShaderAssetDrawer final : public AssetDrawer<ShaderAsset, ShaderAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "Shader"; }

        void DrawContent(Ref<ShaderAsset> assetRef) override
        {
            const auto shader = assetRef->GetShader();

            if (!shader || !shader->CanDrawGeometry())
            {
                ImGui::TextWrapped(
                    "Shader failed to compile, or is missing VSMain or PSMain. Check the console for the compiler output.");
                return;
            }

            const auto& layout = shader->GetMaterialLayout();

            ImGui::TextWrapped("Material properties");
            ImGui::Separator();

            if (layout.Properties.empty())
            {
                ImGui::TextWrapped("None. Declare them in a 'MaterialConstantBuffer' cbuffer.");
            }
            else
            {
                for (const auto& property : layout.Properties)
                    ImGui::BulletText("%s : %s", property.Name.c_str(), GetTypeName(property.Type));
            }

            ImGui::Spacing();
            ImGui::TextWrapped("Material textures");
            ImGui::Separator();

            if (layout.Textures.empty())
                ImGui::TextWrapped("None. Declare Texture2D in registers t0 to t8.");
            else
                for (const auto& texture : layout.Textures)
                    ImGui::BulletText("%s : t%u", texture.Name.c_str(), texture.Slot);
        }

    private:
        static const char* GetTypeName(const ShaderPropertyType type)
        {
            switch (type)
            {
            case ShaderPropertyType::Float: return "float";
            case ShaderPropertyType::Float2: return "float2";
            case ShaderPropertyType::Float3: return "float3";
            case ShaderPropertyType::Float4: return "float4";
            case ShaderPropertyType::Int: return "int";
            case ShaderPropertyType::Bool: return "bool";
            default: return "unknown";
            }
        }
    };
}
