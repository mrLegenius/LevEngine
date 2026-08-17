#include "pch.h"
#include "AssetThumbnailCache.h"

#include "Renderer/Pipeline/Texture.h"

//<--- The editor carries its own stb implementation. The engine compiles one into LevEngine.dll but does
//<--- not export it, and previews are an editor concern, so nothing in the engine has to change ---<<
#define STB_IMAGE_IMPLEMENTATION
#include "stb/include/stb_image.h"

namespace LevEngine::Editor
{
    namespace
    {
        struct ThumbnailSize
        {
            int Width;
            int Height;
        };

        ThumbnailSize FitToThumbnail(const int width, const int height)
        {
            const int longestEdge = width > height ? width : height;

            if (longestEdge <= static_cast<int>(AssetThumbnailCache::k_ThumbnailSize))
                return { width, height };

            const float scale =
                static_cast<float>(AssetThumbnailCache::k_ThumbnailSize) / static_cast<float>(longestEdge);

            const int scaledWidth = static_cast<int>(static_cast<float>(width) * scale);
            const int scaledHeight = static_cast<int>(static_cast<float>(height) * scale);

            //<--- A very wide or very tall image would otherwise round an edge down to nothing ---<<
            return { scaledWidth > 0 ? scaledWidth : 1, scaledHeight > 0 ? scaledHeight : 1 };
        }

        //<--- Box filter down to the target size. Always four channels in, RGBA8 out, so a preview costs
        //<--- the same regardless of what the source was ---<<
        Vector<uint8_t> Downsample(const void* source, const bool isHDR,
                                   const int width, const int height,
                                   const ThumbnailSize& target)
        {
            const auto* bytes = static_cast<const uint8_t*>(source);
            const auto* floats = static_cast<const float*>(source);

            Vector<uint8_t> result(static_cast<size_t>(target.Width) * target.Height * 4);

            for (int y = 0; y < target.Height; ++y)
            {
                const int sourceYBegin = y * height / target.Height;
                int sourceYEnd = (y + 1) * height / target.Height;
                if (sourceYEnd <= sourceYBegin) sourceYEnd = sourceYBegin + 1;

                for (int x = 0; x < target.Width; ++x)
                {
                    const int sourceXBegin = x * width / target.Width;
                    int sourceXEnd = (x + 1) * width / target.Width;
                    if (sourceXEnd <= sourceXBegin) sourceXEnd = sourceXBegin + 1;

                    float accumulated[4]{};
                    int sampleCount = 0;

                    for (int sourceY = sourceYBegin; sourceY < sourceYEnd; ++sourceY)
                    {
                        for (int sourceX = sourceXBegin; sourceX < sourceXEnd; ++sourceX)
                        {
                            const size_t index = (static_cast<size_t>(sourceY) * width + sourceX) * 4;

                            for (int channel = 0; channel < 4; ++channel)
                            {
                                accumulated[channel] += isHDR
                                    ? floats[index + channel]
                                    : static_cast<float>(bytes[index + channel]) / 255.0f;
                            }

                            ++sampleCount;
                        }
                    }

                    const size_t out = (static_cast<size_t>(y) * target.Width + x) * 4;

                    for (int channel = 0; channel < 4; ++channel)
                    {
                        float value = accumulated[channel] / static_cast<float>(sampleCount);

                        //<--- An HDR value runs past 1, so without a curve every preview of an
                        //<--- environment map clamps to a white square. Alpha is left alone: tone
                        //<--- mapping a fully opaque pixel would make it translucent ---<<
                        if (isHDR && channel < 3)
                        {
                            value = value / (value + 1.0f);
                            value = powf(value, 1.0f / 2.2f);
                        }

                        if (value < 0) value = 0;
                        if (value > 1) value = 1;

                        result[out + channel] = static_cast<uint8_t>(value * 255.0f + 0.5f);
                    }
                }
            }

            return result;
        }
    }

    bool AssetThumbnailCache::TryGet(const Path& path, Ref<Texture>& outTexture)
    {
        const auto it = s_Thumbnails.find(ToString(path));
        if (it == s_Thumbnails.end()) return false;

        outTexture = it->second;
        return true;
    }

    Ref<Texture> AssetThumbnailCache::Load(const Path& path)
    {
        LEV_PROFILE_FUNCTION();

        const String key = ToString(path);
        const std::string pathString = path.string();

        //<--- Matches the engine's loader, and the browser draws its cells with a flipped V ---<<
        stbi_set_flip_vertically_on_load(1);

        const bool isHDR = stbi_is_hdr(pathString.c_str()) != 0;

        int width = 0;
        int height = 0;
        int channels = 0;

        //<--- Four channels requested, so the downsample never has to care about the source layout ---<<
        void* data = isHDR
            ? static_cast<void*>(stbi_loadf(pathString.c_str(), &width, &height, &channels, 4))
            : static_cast<void*>(stbi_load(pathString.c_str(), &width, &height, &channels, 4));

        if (!data || width <= 0 || height <= 0)
        {
            if (data) stbi_image_free(data);

            //<--- Cached as empty so a file that cannot be decoded is not retried every frame ---<<
            Log::CoreWarning("Failed to create a thumbnail for '{0}'", key);
            s_Thumbnails[key] = nullptr;

            return nullptr;
        }

        const ThumbnailSize target = FitToThumbnail(width, height);
        const Vector<uint8_t> pixels = Downsample(data, isHDR, width, height, target);

        //<--- Released before the texture is made, so the full sized image and the preview are never
        //<--- both resident. This is the whole point of the cache ---<<
        stbi_image_free(data);

        const Texture::TextureFormat format{ Texture::Components::RGBA, Texture::Type::SRGB };

        Ref<Texture> thumbnail = Texture::CreateTexture2D(
            static_cast<uint16_t>(target.Width), static_cast<uint16_t>(target.Height), 1,
            format, const_cast<uint8_t*>(pixels.data()));

        s_Thumbnails[key] = thumbnail;

        return thumbnail;
    }

    void AssetThumbnailCache::Shutdown()
    {
        s_Thumbnails.clear();
    }
}
