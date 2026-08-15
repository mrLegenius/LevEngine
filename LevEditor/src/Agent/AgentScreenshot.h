#pragma once

namespace LevEngine
{
    class Texture;
}

namespace LevEngine::Editor::AgentScreenshot
{
    struct Result
    {
        bool IsOk = false;
        String Error;
        Path SavedTo;
        int Width = 0;
        int Height = 0;
    };

    // Saves a render texture as a PNG by reading it back off the GPU. Nothing about the window
    // matters: no focus is taken, no other window can end up in the image, and the size is the size
    // the frame was rendered at rather than whatever the window happens to be.
    //
    // A requested width or height box filters the readback down to it, which is what makes two
    // captures from different runs comparable.
    Result SaveTexture(const Ref<Texture>& texture, const Path& path, int requestedWidth, int requestedHeight);

    //<--- The whole editor window, chrome and panels included, through PrintWindow so the window
    //does not have to be in front ---<<
    Result SaveWindow(const Path& path, int requestedWidth, int requestedHeight);
}
