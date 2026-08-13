#include "levpch.h"
#include "TextureLibrary.h"
#include "Renderer/Pipeline/Texture.h"

LevEngine::Ref<LevEngine::Texture> LevEngine::TextureLibrary::GetWhiteTexture()
{
    if (!m_WhiteTexture)
    {
        uint8_t data[3] = {255, 255, 255};
        m_WhiteTexture = Texture::CreateTexture2D(1, 1, 1, Texture::TextureFormat(), &data[0]);
    }

    return m_WhiteTexture;
}

LevEngine::Ref<LevEngine::Texture> LevEngine::TextureLibrary::GetBlackTexture()
{
    if (!m_BlackTexture)
    {
        uint8_t data[3] = {0, 0, 0};
        m_BlackTexture = Texture::CreateTexture2D(1, 1, 1, Texture::TextureFormat(), &data[0]);
    }

    return m_BlackTexture;
}

LevEngine::Ref<LevEngine::Texture> LevEngine::TextureLibrary::GetEmptyNormalMap()
{
    if (!m_EmptyNormalMap)
    {
        uint8_t data[3] = {128, 128, 255};
        m_EmptyNormalMap = Texture::CreateTexture2D(1, 1, 1, Texture::TextureFormat(), &data[0]);
    }

    return m_EmptyNormalMap;
}

LevEngine::Ref<LevEngine::Texture> LevEngine::TextureLibrary::GetTexture(const String& path)
{
    if (const auto texture = m_TextureMap.find(path); texture != m_TextureMap.end())
        return texture->second;

    auto newTexture = Texture::Create(path);
    m_TextureMap.emplace(path, newTexture);
    return newTexture;
}

void LevEngine::TextureLibrary::Shutdown()
{
    m_TextureMap.clear();

    m_WhiteTexture.reset();
    m_BlackTexture.reset();
    m_EmptyNormalMap.reset();
}
