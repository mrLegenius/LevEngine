#pragma once
#include <string>
#include <filesystem>

#include "Katamari/ObjLoader.h"
#include "Renderer/Texture.h"
#include "Renderer/Material.h"
#include "Renderer/Shader.h"

using namespace LevEngine;

static std::filesystem::path resources = std::filesystem::path("resources");

static std::string GetShaderPath(const std::string& name) { return (resources / "Shaders" / name).string(); }
static std::string GetTexturePath(const std::string& name) { return (resources / "Textures" / name).string(); }
static std::string GetModelPath(const std::string& name) { return (resources / "Models" / name).string(); }

struct LavaRockAssets
{
	static auto Mesh()
	{
		static auto mesh = ObjLoader().LoadMesh(GetModelPath("lava_rock.obj"));
		return mesh;
	}

	static auto AmbientTexture()
	{
		static auto texture = Texture::Create(GetTexturePath("LavaRock/ambient.jpg"));
		return texture;
	}

	static auto EmissiveTexture()
	{
		static auto texture = Texture::Create(GetTexturePath("LavaRock/emissive.jpg"));
		return texture;
	}

	static auto SpecularTexture()
	{
		static auto texture = Texture::Create(GetTexturePath("LavaRock/specular.jpg"));
		return texture;
	}


	static auto NormalTexture()
	{
		static auto texture = Texture::Create(GetTexturePath("LavaRock/normal.jpg"));
		return texture;
	}
};

namespace ShaderAssets
{
    inline auto Lit()
    {
        static Ref<Shader> shader;
        if (shader) return shader;

        shader = Shader::Create(GetShaderPath("Lit.hlsl"));
        shader->SetLayout({
        { ShaderDataType::Float3, "POSITION" },
        { ShaderDataType::Float3, "NORMAL" },
        { ShaderDataType::Float2, "UV" },
            });

        return shader;
    }

	inline auto Unlit()
	{
        static Ref<Shader> shader;
        if (shader) return shader;

        shader = Shader::Create(GetShaderPath("Unlit.hlsl"));
        shader->SetLayout({
        { ShaderDataType::Float3, "POSITION" },
		{ ShaderDataType::Float3, "NORMAL" },
		{ ShaderDataType::Float2, "UV" },
        });

        return shader;
	}

	inline auto Skybox()
    {
	    static Ref<Shader> shader;
	    if (shader) return shader;

	    shader = Shader::Create(GetShaderPath("Skybox.hlsl"));
        shader->SetLayout({{ ShaderDataType::Float3, "POSITION" }});

        return shader;
    }

    inline auto ShadowPass()
    {
        static Ref<Shader> shader;
        if (shader) return shader;

        shader = Shader::Create(GetShaderPath("ShadowPass.hlsl"));
        shader->SetLayout({
        { ShaderDataType::Float3, "POSITION" },
        { ShaderDataType::Float3, "NORMAL" },
        { ShaderDataType::Float2, "UV" },
        });

        return shader;
    }

    inline auto CascadeShadowPass()
    {
		LEV_PROFILE_FUNCTION();

        static Ref<Shader> shader;
        if (shader) return shader;

        shader = Shader::Create(GetShaderPath("CascadeShadowPass.hlsl"), Shader::Type::Vertex | Shader::Type::Geometry);
        shader->SetLayout({
        { ShaderDataType::Float3, "POSITION" },
        { ShaderDataType::Float3, "NORMAL" },
        { ShaderDataType::Float2, "UV" },
            });

        return shader;
    }

	inline auto GBufferPass()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("GBufferPass.hlsl"));
		shader->SetLayout({
		{ ShaderDataType::Float3, "POSITION" },
		{ ShaderDataType::Float3, "NORMAL" },
		{ ShaderDataType::Float2, "UV" },
			});

		return shader;
	}

	inline auto DeferredVertexOnly()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("DeferredLightningPass.hlsl"), Shader::Type::Vertex);
		shader->SetLayout({
		{ ShaderDataType::Float3, "POSITION" },
		{ ShaderDataType::Float3, "NORMAL" },
		{ ShaderDataType::Float2, "UV" },
			});

		return shader;
	}

	inline auto DeferredPointLight()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("DeferredLightningPass.hlsl"));
		shader->SetLayout({
		{ ShaderDataType::Float3, "POSITION" },
		{ ShaderDataType::Float3, "NORMAL" },
		{ ShaderDataType::Float2, "UV" },
			});

		return shader;
	}

	inline auto DeferredQuadRender()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("ForwardQuadRender.hlsl"));
		shader->SetLayout({
		{ ShaderDataType::Float3, "POSITION" },
		{ ShaderDataType::Float3, "NORMAL" },
		{ ShaderDataType::Float2, "UV" },
			});

		return shader;
	}

	inline auto Particles()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/Particles.hlsl"), Shader::Type::Vertex | Shader::Type::Geometry | Shader::Type::Pixel);

		return shader;
	}

	inline auto ParticlesCompute()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/ParticlesCompute.hlsl"), Shader::Type::Compute);

		return shader;
	}

	inline auto ParticlesEmitter()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/ParticlesEmitter.hlsl"), Shader::Type::Compute);

		return shader;
	}

	inline auto BitonicSort()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/BitonicSort.hlsl"), Shader::Type::Compute);

		return shader;
	}

	inline auto BitonicTranspose()
	{
		LEV_PROFILE_FUNCTION();

		static Ref<Shader> shader;
		if (shader) return shader;

		shader = Shader::Create(GetShaderPath("Particles/BitonicTranspose.hlsl"), Shader::Type::Compute);

		return shader;
	}
};

namespace TextureAssets
{
    inline Ref<Texture> Bricks()
    {
        static auto texture = Texture::Create(GetTexturePath("bricks.jpg"));
        return texture;
    }

	inline Ref<Texture> Particle()
	{
		static auto texture = Texture::Create(GetTexturePath("particle.png"));
		return texture;
	}

	inline Ref<Texture> Smoke()
	{
		static auto texture = Texture::Create(GetTexturePath("smoke.png"));
		return texture;
	}

    inline Ref<Texture> Log()
    {
        static auto texture = Texture::Create(GetTexturePath("log.jpg"));
        return texture;
    }

    inline Ref<Texture> Gear()
    {
        static auto texture = Texture::Create(GetTexturePath("gear.png"));
        return texture;
    }

    inline Ref<Texture> Rock()
    {
        static auto texture = Texture::Create(GetTexturePath("rock.tga"));
        return texture;
    }

	inline Ref<Texture> Fire()
	{
		static auto texture = Texture::Create(GetTexturePath("fire.png"));
		return texture;
	}
};

namespace SkyboxAssets
{
    inline auto Test()
    {
        static std::string paths[6] = {
            GetTexturePath("TestSkybox/left.png"), //left
            GetTexturePath("TestSkybox/right.png"), //right
            GetTexturePath("TestSkybox/top.png"), //top
            GetTexturePath("TestSkybox/bottom.png"), //bottom
            GetTexturePath("TestSkybox/back.png"), //back
            GetTexturePath("TestSkybox/front.png"), //front
        };

        static auto texture = Texture::CreateTextureCube(paths);

        return texture;
    }

    inline auto LightBlue()
    {
        static std::string paths[6] = {
            GetTexturePath("LightBlueSkybox/left.png"), //left
            GetTexturePath("LightBlueSkybox/right.png"), //right
            GetTexturePath("LightBlueSkybox/top.png"), //top
            GetTexturePath("LightBlueSkybox/bottom.png"), //bottom
            GetTexturePath("LightBlueSkybox/back.png"), //back
            GetTexturePath("LightBlueSkybox/front.png"), //front
        };

        static auto texture = Texture::CreateTextureCube(paths);

        return texture;
    }
};

namespace MeshAssets
{
	inline auto Log()
	{
        static auto mesh = ObjLoader().LoadMesh(GetModelPath("log.obj"));
        return mesh;
	}

    inline auto Gear()
    {
        static auto mesh = ObjLoader().LoadMesh(GetModelPath("gear.obj"));
        return mesh;
    }

    inline auto Rock()
    {
        static auto mesh = ObjLoader().LoadMesh(GetModelPath("rock.obj"));
        return mesh;
    }

    inline auto Cube()
	{
        static auto mesh = Mesh::CreateCube();
        return mesh;
	}
}

struct Materials
{
	static Material Emerald()
	{
		Vector3 ambient = Vector3(0.0215f, 0.1745f, 0.0215f);
		Vector3 diffuse = Vector3(0.07568f, 0.89f, 0.63f);
		Vector3 specular = Vector3(0.633f, 0.727811f, 0.633f);

		float shininess = 128.0f * 0.6f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Jade()
	{
		Vector3 ambient = Vector3(0.135f, 0.2225f, 0.1575f);
		Vector3 diffuse = Vector3(0.54f, 0.89f, 0.63f);
		Vector3 specular = Vector3(0.316228f, 0.316228f, 0.316228f);

		float shininess = 128.0f * 0.1f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Obsidian()
	{
		Vector3 ambient = Vector3(0.05375f, 0.05f, 0.06625f);
		Vector3 diffuse = Vector3(0.18275f, 0.17f, 0.22525f);
		Vector3 specular = Vector3(0.332741f, 0.328634f, 0.346435f);

		float shininess = 128.0f * 0.3f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Pearl()
	{
		Vector3 ambient = Vector3(0.25f, 0.20725f, 0.20725f);
		Vector3 diffuse = Vector3(1.0f, 0.829f, 0.829f);
		Vector3 specular = Vector3(0.296648f, 0.296648f, 0.296648f);

		float shininess = 128.0f * 0.088f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Ruby()
	{
		Vector3 ambient = Vector3(0.1745f, 0.01175f, 0.01175f);
		Vector3 diffuse = Vector3(0.61424f, 0.04136f, 0.04136f);
		Vector3 specular = Vector3(0.727811f, 0.626959f, 0.626959f);

		float shininess = 128.0f * 0.6f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Turquoise()
	{
		Vector3 ambient = Vector3(0.1f, 0.18725f, 0.1745f);
		Vector3 diffuse = Vector3(0.396f, 0.74151f, 0.69102f);
		Vector3 specular = Vector3(0.297254f, 0.30829f, 0.306678f);

		float shininess = 128.0f * 0.1f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Brass()
	{
		Vector3 ambient = Vector3(0.329412f, 0.223529f, 0.027451f);
		Vector3 diffuse = Vector3(0.780392f, 0.568627f, 0.113725f);
		Vector3 specular = Vector3(0.992157f, 0.941176f, 0.807843f);

		float shininess = 128.0f * 0.21794872f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Bronze()
	{
		Vector3 ambient = Vector3(0.2125f, 0.1275f, 0.054f);
		Vector3 diffuse = Vector3(0.714f, 0.4284f, 0.18144f);
		Vector3 specular = Vector3(0.393548f, 0.271906f, 0.166721f);

		float shininess = 128.0f * 0.2f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Chrome()
	{
		Vector3 ambient = Vector3(0.25f, 0.25f, 0.25f);
		Vector3 diffuse = Vector3(0.4f, 0.4f, 0.4f);
		Vector3 specular = Vector3(0.774597f, 0.774597f, 0.774597f);

		float shininess = 128.0f * 0.6f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Copper()
	{
		Vector3 ambient = Vector3(0.19125f, 0.0735f, 0.0225f);
		Vector3 diffuse = Vector3(0.7038f, 0.27048f, 0.0828f);
		Vector3 specular = Vector3(0.256777f, 0.137622f, 0.086014f);

		float shininess = 128.0f * 0.1f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Gold()
	{
		Vector3 ambient = Vector3(0.24725f, 0.1995f, 0.0745f);
		Vector3 diffuse = Vector3(0.75164f, 0.60648f, 0.22648f);
		Vector3 specular = Vector3(0.628281f, 0.555802f, 0.366065f);

		float shininess = 128.0f * 0.4f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material Silver()
	{
		Vector3 ambient = Vector3(0.19225f, 0.19225f, 0.19225f);
		Vector3 diffuse = Vector3(0.50754f, 0.50754f, 0.50754f);
		Vector3 specular = Vector3(0.508273f, 0.508273f, 0.508273f);

		float shininess = 128.0f * 0.4f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material BlackPlastic()
	{
		Vector3 ambient = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 diffuse = Vector3(0.01f, 0.01f, 0.01f);
		Vector3 specular = Vector3(0.50f, 0.50f, 0.50f);

		float shininess = 128.0f * 0.25f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material CyanPlastic()
	{
		Vector3 ambient = Vector3(0.0f, 0.1f, 0.20725f);
		Vector3 diffuse = Vector3(1.0f, 0.829f, 0.829f);
		Vector3 specular = Vector3(0.296648f, 0.296648f, 0.296648f);

		float shininess = 128.0f * 0.25f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material GreenPlastic()
	{
		Vector3 ambient = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 diffuse = Vector3(0.1f, 0.35f, 0.1f);
		Vector3 specular = Vector3(0.45f, 0.55f, 0.45f);

		float shininess = 128.0f * 0.25f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material RedPlastic()
	{
		Vector3 ambient = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 diffuse = Vector3(0.5f, 0.0f, 0.0f);
		Vector3 specular = Vector3(0.7f, 0.6f, 0.6f);

		float shininess = 128.0f * 0.25f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material WhitePlastic()
	{
		Vector3 ambient = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 diffuse = Vector3(0.55f, 0.55f, 0.55f);
		Vector3 specular = Vector3(0.70f, 0.70f, 0.70f);

		float shininess = 128.0f * 0.25f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material YellowPlastic()
	{
		Vector3 ambient = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 diffuse = Vector3(0.5f, 0.5f, 0.0f);
		Vector3 specular = Vector3(0.60f, 0.60f, 0.50f);

		float shininess = 128.0f * 0.25f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material BlackRubber()
	{
		Vector3 ambient = Vector3(0.02f, 0.02f, 0.02f);
		Vector3 diffuse = Vector3(0.01f, 0.01f, 0.01f);
		Vector3 specular = Vector3(0.4f, 0.4f, 0.4f);

		float shininess = 128.0f * 0.078125f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material CyanRubber()
	{
		Vector3 ambient = Vector3(0.0f, 0.05f, 0.05f);
		Vector3 diffuse = Vector3(0.4f, 0.5f, 0.5f);
		Vector3 specular = Vector3(0.04f, 0.7f, 0.7f);

		float shininess = 128.0f * 0.078125f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material GreenRubber()
	{
		Vector3 ambient = Vector3(0.0f, 0.05f, 0.0f);
		Vector3 diffuse = Vector3(0.4f, 0.5f, 0.4f);
		Vector3 specular = Vector3(0.04f, 0.7f, 0.04f);

		float shininess = 128.0f * 0.078125f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material RedRubber()
	{
		Vector3 ambient = Vector3(0.05f, 0.0f, 0.0f);
		Vector3 diffuse = Vector3(0.5f, 0.4f, 0.4f);
		Vector3 specular = Vector3(0.7f, 0.04f, 0.04f);

		float shininess = 128.0f * 0.078125f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material WhiteRubber()
	{
		Vector3 ambient = Vector3(0.05f, 0.05f, 0.05f);
		Vector3 diffuse = Vector3(0.5f, 0.5f, 0.5f);
		Vector3 specular = Vector3(0.7f, 0.7f, 0.7f);

		float shininess = 128.0f * 0.078125f;

		return { ambient, diffuse, specular, shininess };
	}

	static Material YellowRubber()
	{
		Vector3 ambient = Vector3(0.05f, 0.05f, 0.0f);
		Vector3 diffuse = Vector3(0.5f, 0.5f, 0.4f);
		Vector3 specular = Vector3(0.7f, 0.7f, 0.04f);

		float shininess = 128.0f * 0.078125f;

		return { ambient, diffuse, specular, shininess };
	}
};