#include "pch.h"
#include "AgentScreenshot.h"

#include <d3d11_2.h>
#include <DirectXPackedVector.h>
#include <wincodec.h>
#include <wrl/client.h>

#include "Platform/D3D11/D3D11Texture.h"
#include "Renderer/Pipeline/Texture.h"

#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "gdi32.lib")

namespace LevEngine::Editor::AgentScreenshot
{
    namespace
    {
        using Microsoft::WRL::ComPtr;

        struct Image
        {
            Vector<uint8_t> Pixels; //<--- RGBA, 8 bits per channel, tightly packed ---<<
            int Width = 0;
            int Height = 0;
        };

        uint8_t ToByte(const float value)
        {
            return static_cast<uint8_t>(Math::Clamp(value, 0.0f, 1.0f) * 255.0f + 0.5f);
        }

        //<--- A float render target holds linear light, and a PNG is read as if it were sRGB ---<<
        uint8_t EncodeSRGB(const float linear)
        {
            const float clamped = Math::Clamp(linear, 0.0f, 1.0f);
            const float encoded = clamped <= 0.0031308f
                ? clamped * 12.92f
                : 1.055f * std::pow(clamped, 1.0f / 2.4f) - 0.055f;

            return ToByte(encoded);
        }

        bool ConvertRow(const DXGI_FORMAT format, const uint8_t* source, uint8_t* destination, const int width)
        {
            switch (format)
            {
            case DXGI_FORMAT_R8G8B8A8_UNORM:
            case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
                memcpy(destination, source, static_cast<size_t>(width) * 4);
                return true;

            case DXGI_FORMAT_B8G8R8A8_UNORM:
            case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            case DXGI_FORMAT_B8G8R8A8_TYPELESS:
                for (int x = 0; x < width; ++x)
                {
                    destination[x * 4 + 0] = source[x * 4 + 2];
                    destination[x * 4 + 1] = source[x * 4 + 1];
                    destination[x * 4 + 2] = source[x * 4 + 0];
                    destination[x * 4 + 3] = source[x * 4 + 3];
                }
                return true;

            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                {
                    const auto* halves = reinterpret_cast<const DirectX::PackedVector::HALF*>(source);

                    for (int x = 0; x < width; ++x)
                    {
                        for (int channel = 0; channel < 3; ++channel)
                        {
                            const float value = DirectX::PackedVector::XMConvertHalfToFloat(halves[x * 4 + channel]);
                            destination[x * 4 + channel] = EncodeSRGB(value);
                        }

                        destination[x * 4 + 3] = ToByte(
                            DirectX::PackedVector::XMConvertHalfToFloat(halves[x * 4 + 3]));
                    }
                }
                return true;

            case DXGI_FORMAT_R32G32B32A32_FLOAT:
                {
                    const auto* floats = reinterpret_cast<const float*>(source);

                    for (int x = 0; x < width; ++x)
                    {
                        for (int channel = 0; channel < 3; ++channel)
                            destination[x * 4 + channel] = EncodeSRGB(floats[x * 4 + channel]);

                        destination[x * 4 + 3] = ToByte(floats[x * 4 + 3]);
                    }
                }
                return true;

            default:
                return false;
            }
        }

        //<--- Box filter. A screenshot taken to be compared with another one is worth resampling
        //properly, and point sampling a planet's horizon turns it into a staircase ---<<
        Image Resize(const Image& source, const int width, const int height)
        {
            Image result;
            result.Width = width;
            result.Height = height;
            result.Pixels.resize(static_cast<size_t>(width) * height * 4);

            for (int y = 0; y < height; ++y)
            {
                const int sourceY0 = y * source.Height / height;
                const int sourceY1 = Math::Max(sourceY0 + 1, (y + 1) * source.Height / height);

                for (int x = 0; x < width; ++x)
                {
                    const int sourceX0 = x * source.Width / width;
                    const int sourceX1 = Math::Max(sourceX0 + 1, (x + 1) * source.Width / width);

                    uint32_t totals[4] = { 0, 0, 0, 0 };
                    uint32_t count = 0;

                    for (int sourceY = sourceY0; sourceY < sourceY1; ++sourceY)
                    {
                        for (int sourceX = sourceX0; sourceX < sourceX1; ++sourceX)
                        {
                            const size_t index = (static_cast<size_t>(sourceY) * source.Width + sourceX) * 4;

                            for (int channel = 0; channel < 4; ++channel)
                                totals[channel] += source.Pixels[index + channel];

                            count++;
                        }
                    }

                    const size_t destinationIndex = (static_cast<size_t>(y) * width + x) * 4;
                    for (int channel = 0; channel < 4; ++channel)
                        result.Pixels[destinationIndex + channel] = static_cast<uint8_t>(totals[channel] / Math::Max(count, 1u));
                }
            }

            return result;
        }

        Image FitToRequest(const Image& image, const int requestedWidth, const int requestedHeight)
        {
            if (requestedWidth <= 0 && requestedHeight <= 0) return image;
            if (image.Width <= 0 || image.Height <= 0) return image;

            //<--- One of the two is enough, the other follows the aspect ratio ---<<
            int width = requestedWidth;
            int height = requestedHeight;

            if (width <= 0)
                width = Math::Max(1, height * image.Width / image.Height);

            if (height <= 0)
                height = Math::Max(1, width * image.Height / image.Width);

            if (width == image.Width && height == image.Height) return image;

            return Resize(image, width, height);
        }

        bool WritePng(const Image& image, const Path& path, String& outError)
        {
            const HRESULT initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
            const bool shouldUninitialize = SUCCEEDED(initialized);

            const auto fail = [&](const char* message)
            {
                outError = message;
                if (shouldUninitialize) CoUninitialize();
                return false;
            };

            ComPtr<IWICImagingFactory> factory;
            if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&factory))))
                return fail("could not create the WIC imaging factory");

            std::error_code errorCode;
            create_directories(path.parent_path(), errorCode);

            ComPtr<IWICStream> stream;
            if (FAILED(factory->CreateStream(&stream)) || FAILED(stream->InitializeFromFilename(path.c_str(), GENERIC_WRITE)))
                return fail("could not open the file for writing");

            ComPtr<IWICBitmapEncoder> encoder;
            if (FAILED(factory->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder))
                || FAILED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache)))
                return fail("could not create the PNG encoder");

            ComPtr<IWICBitmapFrameEncode> frame;
            ComPtr<IPropertyBag2> properties;
            if (FAILED(encoder->CreateNewFrame(&frame, &properties)) || FAILED(frame->Initialize(properties.Get())))
                return fail("could not create the PNG frame");

            if (FAILED(frame->SetSize(image.Width, image.Height)))
                return fail("could not set the image size");

            WICPixelFormatGUID format = GUID_WICPixelFormat32bppRGBA;
            if (FAILED(frame->SetPixelFormat(&format)))
                return fail("could not set the pixel format");

            const UINT stride = static_cast<UINT>(image.Width) * 4;
            if (FAILED(frame->WritePixels(image.Height, stride, static_cast<UINT>(image.Pixels.size()),
                const_cast<BYTE*>(image.Pixels.data()))))
                return fail("could not write the pixels");

            if (FAILED(frame->Commit()) || FAILED(encoder->Commit()))
                return fail("could not finish the file");

            if (shouldUninitialize) CoUninitialize();

            return true;
        }

        Result Save(const Image& image, const Path& path, const int requestedWidth, const int requestedHeight)
        {
            Result result;

            const Image fitted = FitToRequest(image, requestedWidth, requestedHeight);

            String error;
            if (!WritePng(fitted, path, error))
            {
                result.Error = error;
                return result;
            }

            result.IsOk = true;
            result.SavedTo = path;
            result.Width = fitted.Width;
            result.Height = fitted.Height;

            return result;
        }
    }

    Result SaveTexture(const Ref<Texture>& texture, const Path& path,
                       const int requestedWidth, const int requestedHeight)
    {
        Result result;

        const auto d3dTexture = CastRef<D3D11Texture>(texture);
        if (!d3dTexture)
        {
            result.Error = "there is nothing rendered to capture";
            return result;
        }

        ID3D11Resource* resource = d3dTexture->GetTextureResource();
        if (!resource)
        {
            result.Error = "the render texture has no GPU resource";
            return result;
        }

        ComPtr<ID3D11Texture2D> source;
        if (FAILED(resource->QueryInterface(IID_PPV_ARGS(&source))))
        {
            result.Error = "the render texture is not a 2D texture";
            return result;
        }

        D3D11_TEXTURE2D_DESC description{};
        source->GetDesc(&description);

        ComPtr<ID3D11Device> device;
        source->GetDevice(&device);

        ComPtr<ID3D11DeviceContext> context;
        device->GetImmediateContext(&context);

        //<--- A render target cannot be read by the CPU, so it is copied into one that can ---<<
        D3D11_TEXTURE2D_DESC stagingDescription = description;
        stagingDescription.Usage = D3D11_USAGE_STAGING;
        stagingDescription.BindFlags = 0;
        stagingDescription.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        stagingDescription.MiscFlags = 0;
        stagingDescription.MipLevels = 1;
        stagingDescription.ArraySize = 1;

        ComPtr<ID3D11Texture2D> staging;
        if (FAILED(device->CreateTexture2D(&stagingDescription, nullptr, &staging)))
        {
            result.Error = "could not create a readback texture";
            return result;
        }

        if (description.SampleDesc.Count > 1)
        {
            D3D11_TEXTURE2D_DESC resolvedDescription = description;
            resolvedDescription.SampleDesc.Count = 1;
            resolvedDescription.SampleDesc.Quality = 0;
            resolvedDescription.Usage = D3D11_USAGE_DEFAULT;
            resolvedDescription.BindFlags = 0;
            resolvedDescription.CPUAccessFlags = 0;
            resolvedDescription.MiscFlags = 0;

            ComPtr<ID3D11Texture2D> resolved;
            if (FAILED(device->CreateTexture2D(&resolvedDescription, nullptr, &resolved)))
            {
                result.Error = "could not resolve the multisampled render texture";
                return result;
            }

            context->ResolveSubresource(resolved.Get(), 0, source.Get(), 0, description.Format);
            context->CopyResource(staging.Get(), resolved.Get());
        }
        else
        {
            context->CopyResource(staging.Get(), source.Get());
        }

        D3D11_MAPPED_SUBRESOURCE mapped{};
        if (FAILED(context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped)))
        {
            result.Error = "could not read the render texture back";
            return result;
        }

        Image image;
        image.Width = static_cast<int>(description.Width);
        image.Height = static_cast<int>(description.Height);
        image.Pixels.resize(static_cast<size_t>(image.Width) * image.Height * 4);

        bool converted = true;
        for (int y = 0; y < image.Height && converted; ++y)
        {
            const auto* row = static_cast<const uint8_t*>(mapped.pData) + static_cast<size_t>(y) * mapped.RowPitch;
            converted = ConvertRow(description.Format, row, image.Pixels.data() + static_cast<size_t>(y) * image.Width * 4,
                image.Width);
        }

        context->Unmap(staging.Get(), 0);

        if (!converted)
        {
            result.Error = Format("unsupported render texture format {0}", static_cast<int>(description.Format));
            return result;
        }

        return Save(image, path, requestedWidth, requestedHeight);
    }

    Result SaveWindow(const Path& path, const int requestedWidth, const int requestedHeight)
    {
        Result result;

        const auto window = static_cast<HWND>(App::Get().GetWindow().GetNativeWindow());
        if (!window)
        {
            result.Error = "the editor has no window";
            return result;
        }

        RECT bounds{};
        if (!GetClientRect(window, &bounds))
        {
            result.Error = "could not measure the window";
            return result;
        }

        const int width = bounds.right - bounds.left;
        const int height = bounds.bottom - bounds.top;

        if (width <= 0 || height <= 0)
        {
            result.Error = "the window has no size, it is probably minimized";
            return result;
        }

        const HDC windowDC = GetDC(window);
        const HDC memoryDC = CreateCompatibleDC(windowDC);

        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof info.bmiHeader;
        info.bmiHeader.biWidth = width;
        //<--- Negative, so the rows come back top down like every other image here ---<<
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        void* bits = nullptr;
        const HBITMAP bitmap = CreateDIBSection(memoryDC, &info, DIB_RGB_COLORS, &bits, nullptr, 0);

        const auto cleanup = [&]
        {
            if (bitmap) DeleteObject(bitmap);
            DeleteDC(memoryDC);
            ReleaseDC(window, windowDC);
        };

        if (!bitmap || !bits)
        {
            cleanup();
            result.Error = "could not create the capture bitmap";
            return result;
        }

        const HGDIOBJ previous = SelectObject(memoryDC, bitmap);

        //<--- PW_RENDERFULLCONTENT is what makes this work on a window drawn by the GPU, and it
        //works without the window being in front, unlike copying from the screen ---<<
        constexpr UINT k_RenderFullContent = 0x00000002;
        const BOOL printed = PrintWindow(window, memoryDC, k_RenderFullContent);

        Image image;
        image.Width = width;
        image.Height = height;
        image.Pixels.resize(static_cast<size_t>(width) * height * 4);

        const auto* source = static_cast<const uint8_t*>(bits);
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                const size_t sourceIndex = (static_cast<size_t>(y) * width + x) * 4;
                const size_t destinationIndex = sourceIndex;

                //<--- A DIB is BGRA and its alpha is not meaningful here ---<<
                image.Pixels[destinationIndex + 0] = source[sourceIndex + 2];
                image.Pixels[destinationIndex + 1] = source[sourceIndex + 1];
                image.Pixels[destinationIndex + 2] = source[sourceIndex + 0];
                image.Pixels[destinationIndex + 3] = 255;
            }
        }

        SelectObject(memoryDC, previous);
        cleanup();

        if (!printed)
        {
            result.Error = "PrintWindow failed";
            return result;
        }

        return Save(image, path, requestedWidth, requestedHeight);
    }
}
