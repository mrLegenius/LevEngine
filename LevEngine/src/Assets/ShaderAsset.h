#pragma once
#include "Asset.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Shader/ShaderMacros.h"

namespace LevEngine
{
    class Shader;

    class ShaderAsset : public Asset
    {
    public:
        ShaderAsset(const Path& path, const UUID& uuid, const ShaderMacros& shaderMacros);
        ShaderAsset(const Path& path, const UUID& uuid);
        
        bool WriteDataToFile() const override { return false; }
        bool ReadDataFromFile() const override { return false; }
        const Ref<Shader>& GetShader() const { return m_Shader; }

    protected:
        bool GenerateMeta() override { return false; }
        void SerializeData(YAML::Emitter& out) override { }
        void DeserializeData(const YAML::Node& node) override;

    private:
        Ref<Shader> m_Shader;
        ShaderMacros m_ShaderMacros;
    };
}
