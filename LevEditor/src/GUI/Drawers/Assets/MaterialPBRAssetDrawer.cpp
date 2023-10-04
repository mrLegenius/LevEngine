#include "pch.h"

#include "AssetDrawer.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class MaterialPBRAssetDrawer final : public AssetDrawer<MaterialPBRAsset, MaterialPBRAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "MaterialPBRAsset"; }

        void DrawContent(Ref<MaterialPBRAsset> assetRef) override
        {
            MaterialPBR& material = *static_cast<MaterialPBR*>(&assetRef->GetMaterial());
            GUIUtils::DrawColor3Control("Tint", BindGetter(&MaterialPBR::GetTintColor, &material), BindSetter(&MaterialPBR::SetTintColor, &material));

            GUIUtils::DrawFloatControl("Metallic", BindGetter(&MaterialPBR::GetMetallic, &material), BindSetter(&MaterialPBR::SetMetallic, &material), 0.01f, 0.04f, 1.0f);
            GUIUtils::DrawFloatControl("Roughness", BindGetter(&MaterialPBR::GetRoughness, &material), BindSetter(&MaterialPBR::SetRoughness, &material), 0.01f, 0.0f, 1.0f);

            DrawMaterialTexture("Albedo", material, MaterialPBR::TextureType::Albedo, assetRef->GetAlbedo());
            DrawMaterialTexture("Metallic", material, MaterialPBR::TextureType::Metallic, assetRef->GetMetallic());
            DrawMaterialTexture("Roughness", material, MaterialPBR::TextureType::Roughness, assetRef->GetRoughness());
            DrawMaterialTexture("Normal", material, MaterialPBR::TextureType::Normal, assetRef->GetNormal());
            DrawMaterialTexture("AmbientOcclusion", material, MaterialPBR::TextureType::AmbientOcclusion, assetRef->GetAmbientOcclusion());
            DrawMaterialTexture("Emissive", material, MaterialPBR::TextureType::Emissive, assetRef->GetEmissive());

            auto tiling = material.GetTextureTiling();
            if (GUIUtils::DrawVector2Control("Tiling", tiling, 1, 100))
                material.SetTextureTiling(tiling);

            auto offset = material.GetTextureOffset();
            if (GUIUtils::DrawVector2Control("Offset", offset, 0, 100))
                material.SetTextureOffset(offset);
        }
        
        void DrawMaterialTexture(const String& label, MaterialPBR& MaterialPBR,
            MaterialPBR::TextureType textureType, Ref<TextureAsset>& textureAsset) const
        {
            ImGui::PushID(static_cast<int>(textureType));

            if (GUIUtils::DrawTextureAsset(label, textureAsset))
                MaterialPBR.SetTexture(textureType, textureAsset ? textureAsset->GetTexture() : nullptr);
            ImGui::PopID();
        }
    };
}
