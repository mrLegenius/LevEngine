#pragma once

namespace LevEngine::Editor
{
    //<--- Small previews for image assets, kept apart from the assets themselves.
    //
    //<--- The asset browser used to preview a texture by deserializing it, which meant a 64 pixel cell
    //<--- pulled in the full resource: an 8K environment map decodes to R32G32B32A32_FLOAT, half a
    //<--- gigabyte of RAM and as much VRAM, and nothing ever unloads a texture again. Browsing a folder
    //<--- of them was unusable and the cost was permanent.
    //
    //<--- A preview here is its own small RGBA8 texture. The source is decoded once, downsampled, and
    //<--- released immediately, so what stays resident is tens of kilobytes per asset instead of the
    //<--- whole image. Previews live for the session only; there is no on-disk cache.
    class AssetThumbnailCache
    {
    public:
        //<--- Longest edge of a generated preview. The browser's thumbnail slider stops at 128, so this
        //<--- is as large as a preview can be drawn ---<<
        static constexpr uint32_t k_ThumbnailSize = 128;

        //<--- Cache lookup only, never decodes. Returns whether this path was attempted, so a caller
        //<--- with a per frame budget can tell a ready preview from one it still has to pay for.
        //<--- A file that fails to decode caches an empty result, so it is attempted once and not
        //<--- retried on every frame ---<<
        static bool TryGet(const Path& path, Ref<Texture>& outTexture);

        //<--- Decodes, downsamples and caches. Returns nullptr when the image cannot be read ---<<
        static Ref<Texture> Load(const Path& path);

        //<--- Releases every preview. Has to be called while the render device is still alive,
        //<--- see TextureLibrary::Shutdown ---<<
        static void Shutdown();

    private:
        static inline UnorderedMap<String, Ref<Texture>> s_Thumbnails;
    };
}
