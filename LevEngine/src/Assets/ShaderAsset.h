#pragma once

#include "Kernel/Core.h"
#include "Asset.h"
#include "Renderer/Shader/Shader.h"
#include "Renderer/Shader/ShaderMacros.h"

namespace LevEngine
{
    class Shader;

    // A shader comes in two flavours.
    //
    // The engine's own shaders are owned by the ShaderLibrary: one asset per file and macro set,
    // no UUID and no meta file, because nothing references them by anything but their path.
    //
    // A .hlsl inside a project is imported like every other asset: it gets a UUID and a meta
    // file so a material can point at it. It does not compile a copy of its own -- the
    // permutations a material needs come from the ShaderLibrary, and the default one is what the
    // inspector shows.
    class LEV_API ShaderAsset : public Asset
    {
    public:
        enum class CreateFrom
        {
            //<--- An empty file, for a shader that is about to be written or imported ---<<
            Nothing,
            //<--- A copy of LevResources/Shaders/Templates/CustomShader.hlsl ---<<
            Template,
        };

        //<--- Project asset ---<<
        ShaderAsset(const Path& path, const UUID& uuid);
        ShaderAsset(const Path& path, const UUID& uuid, CreateFrom source);

        //<--- ShaderLibrary asset: owns one compiled permutation, generates no meta ---<<
        ShaderAsset(const Path& path, const UUID& uuid, const ShaderMacros& shaderMacros);

        bool WriteDataToFile() const override { return false; }
        bool ReadDataFromFile() const override { return false; }

        // The compiled shader. Library assets own it, project assets share the default
        // permutation with every material that draws with them.
        [[nodiscard]] Ref<Shader> GetShader() const;

    protected:
        bool GenerateMeta() override { return m_GenerateMeta; }
        void SerializeData(YAML::Emitter& out) override { }
        void DeserializeData(const YAML::Node& node) override;

    private:
        Ref<Shader> m_Shader;
        ShaderMacros m_ShaderMacros;

        bool m_GenerateMeta{true};
    };
}
