#include "pch.h"

#include "AssetDrawer.h"
#include "imgui.h"
#include "Assets/MaterialSimpleAsset.h"
#include "Assets/TextureAsset.h"
#include "GUI/EditorGUI.h"

namespace LevEngine::Editor
{
    class MaterialSimpleAssetDrawer final : public AssetDrawer<MaterialSimpleAsset, MaterialSimpleAssetDrawer>
    {
    protected:
        [[nodiscard]] String GetLabel() const override { return "MaterialSimpleAsset"; }

        void DrawContent(Ref<MaterialSimpleAsset> assetRef) override
        {
            MaterialSimple& material = *static_cast<MaterialSimple*>(&assetRef->GetMaterial());
            EditorGUI::DrawColor3Control("Ambient", BindGetter(&MaterialSimple::GetAmbientColor, &material), BindSetter(&MaterialSimple::SetAmbientColor, &material));
            EditorGUI::DrawColor3Control("Diffuse", BindGetter(&MaterialSimple::GetDiffuseColor, &material), BindSetter(&MaterialSimple::SetDiffuseColor, &material));
            EditorGUI::DrawColor3Control("Specular", BindGetter(&MaterialSimple::GetSpecularColor, &material), BindSetter(&MaterialSimple::SetSpecularColor, &material));
            EditorGUI::DrawColor3Control("Emissive", BindGetter(&MaterialSimple::GetEmissiveColor, &material), BindSetter(&MaterialSimple::SetEmissiveColor, &material));

            EditorGUI::DrawFloatControl("Shininess", BindGetter(&MaterialSimple::GetShininess, &material), BindSetter(&MaterialSimple::SetShininess, &material), 1.0f, 2.0f, 128.0f);

            DrawMaterialTexture("Diffuse", material, MaterialSimple::TextureType::Diffuse, assetRef->GetDiffuse());
            DrawMaterialTexture("Specular", material, MaterialSimple::TextureType::Specular, assetRef->GetSpecular());
            DrawMaterialTexture("Normal", material, MaterialSimple::TextureType::Normal, assetRef->GetNormal());
            DrawMaterialTexture("Emissive", material, MaterialSimple::TextureType::Emissive, assetRef->GetEmissive());

            auto tiling = material.GetTextureTiling();
            if (EditorGUI::DrawVector2Control("Tiling", tiling, 1, 100))
                material.SetTextureTiling(tiling);

            auto offset = material.GetTextureOffset();
            if (EditorGUI::DrawVector2Control("Offset", offset, 0, 100))
                material.SetTextureOffset(offset);
        }
        
        void DrawMaterialTexture(const String& label, MaterialSimple& material,
            MaterialSimple::TextureType textureType, Ref<TextureAsset>& textureAsset) const
        {
            ImGui::PushID(static_cast<int>(textureType));

            if (EditorGUI::DrawTextureAsset(label, textureAsset))
                material.SetTexture(textureType, textureAsset ? textureAsset->GetTexture() : nullptr);
            ImGui::PopID();
        }
    };
}
