#include "pch.h"

#include "AssetDrawer.h"
#include "Assets/MaterialPBRAsset.h"
#include "Assets/TextureAsset.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class MaterialPBRAssetDrawer final : public AssetDrawer<MaterialPBRAsset, MaterialPBRAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "MaterialPBRAsset"; }

        void DrawContent(Ref<MaterialPBRAsset> assetRef) override
        {
            MaterialPBR& material = *static_cast<MaterialPBR*>(&assetRef->GetMaterial());
            EditorGUI::DrawColor3Control("Tint", BindGetter(&MaterialPBR::GetTintColor, &material), BindSetter(&MaterialPBR::SetTintColor, &material));

            EditorGUI::DrawFloatControl("Metallic", BindGetter(&MaterialPBR::GetMetallic, &material), BindSetter(&MaterialPBR::SetMetallic, &material), 0.01f, 0.04f, 1.0f);
            EditorGUI::DrawFloatControl("Roughness", BindGetter(&MaterialPBR::GetRoughness, &material), BindSetter(&MaterialPBR::SetRoughness, &material), 0.01f, 0.0f, 1.0f);
            EditorGUI::DrawCheckBox("Enable Transparency", BindGetter(&MaterialPBR::GetEnableTransparency, &material), BindSetter(&MaterialPBR::SetEnableTransparency, &material));

            DrawMaterialTexture("Albedo", MaterialPBR::TextureType::Albedo, assetRef);
            DrawMaterialTexture("Metallic", MaterialPBR::TextureType::Metallic, assetRef);
            DrawMaterialTexture("Roughness", MaterialPBR::TextureType::Roughness, assetRef);
            DrawMaterialTexture("Normal", MaterialPBR::TextureType::Normal, assetRef);
            DrawMaterialTexture("AmbientOcclusion", MaterialPBR::TextureType::AmbientOcclusion, assetRef);
            DrawMaterialTexture("Emissive", MaterialPBR::TextureType::Emissive, assetRef);

            auto tiling = material.GetTextureTiling();
            if (EditorGUI::DrawVector2Control("Tiling", tiling, 1, 100))
                material.SetTextureTiling(tiling);

            auto offset = material.GetTextureOffset();
            if (EditorGUI::DrawVector2Control("Offset", offset, 0, 100))
                material.SetTextureOffset(offset);
        }

        static void DrawMaterialTexture(const String& label, MaterialPBR::TextureType textureType, const Ref<MaterialPBRAsset>& assetRef)
        {
            ImGui::PushID(static_cast<int>(textureType));

            //TODO: Fix Texture Clear
            auto textureAsset = assetRef->GetTexture(textureType);
            if (EditorGUI::DrawTextureAsset(label, textureAsset))
                assetRef->SetTexture(textureType, textureAsset);
            ImGui::PopID();
        }
    };
}
