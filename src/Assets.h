#pragma once
#include <string>
#include <filesystem>
#include "Katamari/ObjLoader.h"

static std::filesystem::path resources = "./resources";

static std::string GetShaderPath(const std::string& name) { return (resources / "Shaders" / name).string(); }
static std::string GetTexturePath(const std::string& name) { return (resources / "Textures" / name).string(); }
static std::string GetModelPath(const std::string& name) { return (resources / "Models" / name).string(); }

namespace ShaderAssets
{
    inline auto Lit()
    {
        static auto shader = std::make_shared<D3D11Shader>(GetShaderPath("Lit.hlsl"));
        shader->SetLayout({
        { ShaderDataType::Float3, "POSITION" },
        { ShaderDataType::Float3, "NORMAL" },
        { ShaderDataType::Float2, "UV" },
            });

        return shader;
    }

	inline auto Unlit()
	{
        static auto shader = std::make_shared<D3D11Shader>(GetShaderPath("Unlit.hlsl"));
        shader->SetLayout({
        { ShaderDataType::Float3, "POSITION" },
		{ ShaderDataType::Float3, "NORMAL" },
		{ ShaderDataType::Float2, "UV" },
        });

        return shader;
	}

	inline auto Skybox()
    {
        static auto shader = std::make_shared<D3D11Shader>(GetShaderPath("Skybox.hlsl"));
        shader->SetLayout({{ ShaderDataType::Float3, "POSITION" }});

        return shader;
    }
};

namespace TextureAssets
{
    inline std::shared_ptr<D3D11Texture2D> Bricks()
    {
        static auto texture = std::make_shared<D3D11Texture2D>(GetTexturePath("bricks.jpg"));
        return texture;
    }

    inline std::shared_ptr<D3D11Texture2D> Log()
    {
        static auto texture = std::make_shared<D3D11Texture2D>(GetTexturePath("log.jpg"));
        return texture;
    }

    inline std::shared_ptr<D3D11Texture2D> Gear()
    {
        static auto texture = std::make_shared<D3D11Texture2D>(GetTexturePath("gear.png"));
        return texture;
    }

    inline std::shared_ptr<D3D11Texture2D> Rock()
    {
        static auto texture = std::make_shared<D3D11Texture2D>(GetTexturePath("rock.tga"));
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

        static auto texture = std::make_shared<D3D11TextureCube>(paths);

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

        static auto texture = std::make_shared<D3D11TextureCube>(paths);

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
}
