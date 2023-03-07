#pragma once
#include <string>

inline std::shared_ptr<D3D11TextureCube> TestSkyboxTexture()
{
    static std::string paths[6] = {
        "./resources/Textures/TestSkybox/left.png", //left
        "./resources/Textures/TestSkybox/right.png", //right
        "./resources/Textures/TestSkybox/top.png", //top
        "./resources/Textures/TestSkybox/bottom.png", //bottom
        "./resources/Textures/TestSkybox/back.png", //back
        "./resources/Textures/TestSkybox/front.png", //front
    };

    static auto texture = std::make_shared<D3D11TextureCube>(paths);

    return texture;
}

inline std::shared_ptr<D3D11TextureCube> LightBlueSkyboxTexture()
{
    static std::string paths[6] = {
        "./resources/Textures/LightBlueSkybox/left.png", //left
        "./resources/Textures/LightBlueSkybox/right.png", //right
        "./resources/Textures/LightBlueSkybox/top.png", //top
        "./resources/Textures/LightBlueSkybox/bottom.png", //bottom
        "./resources/Textures/LightBlueSkybox/back.png", //back
        "./resources/Textures/LightBlueSkybox/front.png", //front
    };

    static auto texture = std::make_shared<D3D11TextureCube>(paths);

    return texture;
}
