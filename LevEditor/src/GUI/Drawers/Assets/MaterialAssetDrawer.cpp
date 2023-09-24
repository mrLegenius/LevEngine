#include "pch.h"

#include "AssetDrawer.h"
#include "imgui.h"
#include "GUI/GUIUtils.h"

namespace LevEngine::Editor
{
    class MaterialAssetDrawer final : public AssetDrawer<MaterialAsset, MaterialAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "MaterialAsset"; }

        void DrawContent(Ref<MaterialAsset> assetRef) override
        {
            auto& material = assetRef->material;

            GUIUtils::DrawColor3Control("Ambient", BindGetter(&Material::GetAmbientColor, &material), BindSetter(&Material::SetAmbientColor, &material));
            GUIUtils::DrawColor3Control("Diffuse", BindGetter(&Material::GetDiffuseColor, &material), BindSetter(&Material::SetDiffuseColor, &material));
            GUIUtils::DrawColor3Control("Specular", BindGetter(&Material::GetSpecularColor, &material), BindSetter(&Material::SetSpecularColor, &material));
            GUIUtils::DrawColor3Control("Emissive", BindGetter(&Material::GetEmissiveColor, &material), BindSetter(&Material::SetEmissiveColor, &material));

            GUIUtils::DrawFloatControl("Shininess", BindGetter(&Material::GetShininess, &material), BindSetter(&Material::SetShininess, &material), 1.0f, 2.0f, 128.0f);

            DrawMaterialTexture("Diffuse", material, Material::TextureType::Diffuse, assetRef->diffuse);
            DrawMaterialTexture("Specular", material, Material::TextureType::Specular, assetRef->specular);
            DrawMaterialTexture("Normal", material, Material::TextureType::Normal, assetRef->normal);
            DrawMaterialTexture("Emissive", material, Material::TextureType::Emissive, assetRef->emissive);

            auto tiling = material.GetTextureTiling();
            if (GUIUtils::DrawVector2Control("Tiling", tiling, 1, 100))
                material.SetTextureTiling(tiling);

            auto offset = material.GetTextureOffset();
            if (GUIUtils::DrawVector2Control("Offset", offset, 0, 100))
                material.SetTextureOffset(offset);
        }
        
        void DrawMaterialTexture(const String& label, Material& material,
                                                Material::TextureType textureType, Ref<TextureAsset>& textureAsset)
        {
            ImGui::PushID(static_cast<int>(textureType));

            if (GUIUtils::DrawTextureAsset(label, &textureAsset))
                material.SetTexture(textureType, textureAsset ? textureAsset->GetTexture() : nullptr);
            ImGui::PopID();
        }
    };
}
