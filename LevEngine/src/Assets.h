#pragma once

#include <filesystem>

#include "TextureLibrary.h"
#include "DataTypes/Path.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"
#include "Renderer/Shader.h"

using namespace LevEngine;

static Path resources = Path("resources");

static String GetShaderPath(const String& name) { return (resources / "Shaders" / name.c_str()).string().c_str(); }
static String GetTexturePath(const String& name) { return (resources / "Textures" / name.c_str()).string().c_str(); }

namespace ShaderAssets
{
    inline auto Lit()
    {
		LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader;
        if (shader) return shader;

        shader = Shader::Create(GetShaderPath("Lit.hlsl"));
        return shader;
    }

	inline auto Unlit()
	{
		LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader;
        if (shader) return shader;

        shader = Shader::Create(GetShaderPath("Unlit.hlsl"));

        return shader;
	}

	inline auto Skybox()
    {
		LEV_PROFILE_FUNCTION();

	    static Ref<Shader> shader;
	    if (shader) return shader;

	    shader = Shader::Create(GetShaderPath("Skybox.hlsl"));

        return shader;
    }

    inline auto ShadowPass()
    {
		LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader;
        if (shader) return shader;

        shader = Shader::Create(GetShaderPath("ShadowPass.hlsl"));

        return shader;
    }

    inline auto CascadeShadowPass()
    {
		LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader;
        if (shader) return shader;

        shader = Shader::Create(GetShaderPath("CascadeShadowPass.hlsl"), ShaderType::Vertex | ShaderType::Geometry);

        return shader;
    }

	inline auto GBufferPass()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("GBufferPass.hlsl"));

		return shader;
	}

	inline auto DeferredVertexOnly()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("DeferredLightningPass.hlsl"), ShaderType::Vertex);
		return shader;
	}

	inline auto DeferredPointLight()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("DeferredLightningPass.hlsl"));

		return shader;
	}

	inline auto DeferredQuadRender()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("ForwardQuadRender.hlsl"));

		return shader;
	}

	inline auto Particles()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/Particles.hlsl"), ShaderType::Vertex | ShaderType::Geometry | ShaderType::Pixel);

		return shader;
	}

	inline auto ParticlesCompute()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/ParticlesCompute.hlsl"), ShaderType::Compute);

		return shader;
	}

	inline auto ParticlesEmitter()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/ParticlesEmitter.hlsl"), ShaderType::Compute);

		return shader;
	}

	inline auto BitonicSort()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/BitonicSort.hlsl"), ShaderType::Compute);

		return shader;
	}

	inline auto BitonicTranspose()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/BitonicTranspose.hlsl"), ShaderType::Compute);

		return shader;
	}
}

namespace TextureAssets
{
	inline Ref<Texture> Particle()
	{
		static auto texture = TextureLibrary::GetTexture(GetTexturePath("particle.png"));
		return texture;
	}
}

struct Materials
{
	static Material Emerald()
	{
		const auto ambient = Color( 0.0215f, 0.1745f, 0.0215f);
		const auto diffuse = Color(0.07568f, 0.89f, 0.63f);
		const auto specular = Color(0.633f, 0.727811f, 0.633f);

		const float shininess = 128.0f * 0.6f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Jade()
	{
		auto ambient = Color(0.135f, 0.2225f, 0.1575f);
		auto diffuse = Color(0.54f, 0.89f, 0.63f);
		auto specular = Color(0.316228f, 0.316228f, 0.316228f);

		float shininess = 128.0f * 0.1f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Obsidian()
	{
		auto ambient = Color(0.05375f, 0.05f, 0.06625f);
		auto diffuse = Color(0.18275f, 0.17f, 0.22525f);
		auto specular = Color(0.332741f, 0.328634f, 0.346435f);

		float shininess = 128.0f * 0.3f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Pearl()
	{
		auto ambient = Color(0.25f, 0.20725f, 0.20725f);
		auto diffuse = Color(1.0f, 0.829f, 0.829f);
		auto specular = Color(0.296648f, 0.296648f, 0.296648f);

		float shininess = 128.0f * 0.088f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Ruby()
	{
		auto ambient = Color(0.1745f, 0.01175f, 0.01175f);
		auto diffuse = Color(0.61424f, 0.04136f, 0.04136f);
		auto specular = Color(0.727811f, 0.626959f, 0.626959f);

		float shininess = 128.0f * 0.6f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Turquoise()
	{
		auto ambient = Color(0.1f, 0.18725f, 0.1745f);
		auto diffuse = Color(0.396f, 0.74151f, 0.69102f);
		auto specular = Color(0.297254f, 0.30829f, 0.306678f);

		float shininess = 128.0f * 0.1f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Brass()
	{
		auto ambient = Color(0.329412f, 0.223529f, 0.027451f);
		auto diffuse = Color(0.780392f, 0.568627f, 0.113725f);
		auto specular = Color(0.992157f, 0.941176f, 0.807843f);

		float shininess = 128.0f * 0.21794872f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Bronze()
	{
		auto ambient = Color(0.2125f, 0.1275f, 0.054f);
		auto diffuse = Color(0.714f, 0.4284f, 0.18144f);
		auto specular = Color(0.393548f, 0.271906f, 0.166721f);

		float shininess = 128.0f * 0.2f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Chrome()
	{
		auto ambient = Color(0.25f, 0.25f, 0.25f);
		auto diffuse = Color(0.4f, 0.4f, 0.4f);
		auto specular = Color(0.774597f, 0.774597f, 0.774597f);

		float shininess = 128.0f * 0.6f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Copper()
	{
		auto ambient = Color(0.19125f, 0.0735f, 0.0225f);
		auto diffuse = Color(0.7038f, 0.27048f, 0.0828f);
		auto specular = Color(0.256777f, 0.137622f, 0.086014f);

		float shininess = 128.0f * 0.1f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Gold()
	{
		auto ambient = Color(0.24725f, 0.1995f, 0.0745f);
		auto diffuse = Color(0.75164f, 0.60648f, 0.22648f);
		auto specular = Color(0.628281f, 0.555802f, 0.366065f);

		float shininess = 128.0f * 0.4f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material Silver()
	{
		auto ambient = Color(0.19225f, 0.19225f, 0.19225f);
		auto diffuse = Color(0.50754f, 0.50754f, 0.50754f);
		auto specular = Color(0.508273f, 0.508273f, 0.508273f);

		float shininess = 128.0f * 0.4f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material BlackPlastic()
	{
		auto ambient = Color(0.0f, 0.0f, 0.0f);
		auto diffuse = Color(0.01f, 0.01f, 0.01f);
		auto specular = Color(0.50f, 0.50f, 0.50f);

		float shininess = 128.0f * 0.25f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material CyanPlastic()
	{
		auto ambient = Color(0.0f, 0.1f, 0.20725f);
		auto diffuse = Color(1.0f, 0.829f, 0.829f);
		auto specular = Color(0.296648f, 0.296648f, 0.296648f);

		float shininess = 128.0f * 0.25f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material GreenPlastic()
	{
		auto ambient = Color(0.0f, 0.0f, 0.0f);
		auto diffuse = Color(0.1f, 0.35f, 0.1f);
		auto specular = Color(0.45f, 0.55f, 0.45f);

		float shininess = 128.0f * 0.25f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material RedPlastic()
	{
		auto ambient = Color(0.0f, 0.0f, 0.0f);
		auto diffuse = Color(0.5f, 0.0f, 0.0f);
		auto specular = Color(0.7f, 0.6f, 0.6f);

		float shininess = 128.0f * 0.25f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material WhitePlastic()
	{
		auto ambient = Color(0.0f, 0.0f, 0.0f);
		auto diffuse = Color(0.55f, 0.55f, 0.55f);
		auto specular = Color(0.70f, 0.70f, 0.70f);

		float shininess = 128.0f * 0.25f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material YellowPlastic()
	{
		auto ambient = Color(0.0f, 0.0f, 0.0f);
		auto diffuse = Color(0.5f, 0.5f, 0.0f);
		auto specular = Color(0.60f, 0.60f, 0.50f);

		float shininess = 128.0f * 0.25f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material BlackRubber()
	{
		auto ambient = Color(0.02f, 0.02f, 0.02f);
		auto diffuse = Color(0.01f, 0.01f, 0.01f);
		auto specular = Color(0.4f, 0.4f, 0.4f);

		float shininess = 128.0f * 0.078125f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material CyanRubber()
	{
		auto ambient = Color(0.0f, 0.05f, 0.05f);
		auto diffuse = Color(0.4f, 0.5f, 0.5f);
		auto specular = Color(0.04f, 0.7f, 0.7f);

		float shininess = 128.0f * 0.078125f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material GreenRubber()
	{
		auto ambient = Color(0.0f, 0.05f, 0.0f);
		auto diffuse = Color(0.4f, 0.5f, 0.4f);
		auto specular = Color(0.04f, 0.7f, 0.04f);

		float shininess = 128.0f * 0.078125f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material RedRubber()
	{
		auto ambient = Color(0.05f, 0.0f, 0.0f);
		auto diffuse = Color(0.5f, 0.4f, 0.4f);
		auto specular = Color(0.7f, 0.04f, 0.04f);

		float shininess = 128.0f * 0.078125f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material WhiteRubber()
	{
		auto ambient = Color(0.05f, 0.05f, 0.05f);
		auto diffuse = Color(0.5f, 0.5f, 0.5f);
		auto specular = Color(0.7f, 0.7f, 0.7f);

		float shininess = 128.0f * 0.078125f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}

	static Material YellowRubber()
	{
		auto ambient = Color(0.05f, 0.05f, 0.0f);
		auto diffuse = Color(0.5f, 0.5f, 0.4f);
		auto specular = Color(0.7f, 0.7f, 0.04f);

		float shininess = 128.0f * 0.078125f;

		Material material{};
		material.SetAmbientColor(ambient);
		material.SetDiffuseColor(diffuse);
		material.SetSpecularColor(specular);
		material.SetShininess(shininess);

		return material;
	}
};