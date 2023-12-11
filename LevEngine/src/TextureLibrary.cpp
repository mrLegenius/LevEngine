#include "levpch.h"
#include "TextureLibrary.h"
#include "Renderer/Texture.h"

LevEngine::Ref<LevEngine::Texture> LevEngine::TextureLibrary::GetWhiteTexture()
{
    static uint8_t data[3] = {255, 255, 255};
    static Ref<Texture> emptyTexture = Texture::CreateTexture2D(1, 1, 1, Texture::TextureFormat(), &data[0]);
    return emptyTexture;
}

LevEngine::Ref<LevEngine::Texture> LevEngine::TextureLibrary::GetBlackTexture()
{
    static uint8_t data[3] = {0, 0, 0};
    static Ref<Texture> emptyTexture = Texture::CreateTexture2D(1, 1, 1, Texture::TextureFormat(), &data[0]);
    return emptyTexture;
}

LevEngine::Ref<LevEngine::Texture> LevEngine::TextureLibrary::GetEmptyNormalMap()
{
    static uint8_t data[3] = {128, 128, 255};
    static Ref<Texture> emptyTexture = Texture::CreateTexture2D(1, 1, 1, Texture::TextureFormat(), &data[0]);
    return emptyTexture;
}

LevEngine::Ref<LevEngine::Texture> LevEngine::TextureLibrary::GetTexture(const String& path)
{
    if (const auto texture = m_TextureMap.find(path); texture != m_TextureMap.end())
        return texture->second;

    auto newTexture = Texture::Create(path);
    m_TextureMap.emplace(path, newTexture);
    return newTexture;
}
