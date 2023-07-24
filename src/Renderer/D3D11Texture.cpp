#include "pch.h"
#include "D3D11Texture.h"

#include <cassert>
#include <wrl/client.h>

#include "d3d11.h"
#include "../../external/stb/include/stb_image.h"
#include "Kernel/Math.h"
namespace LevEngine
{
extern ID3D11DeviceContext* context;
extern Microsoft::WRL::ComPtr<ID3D11Device> device;

DXGI_FORMAT ConvertFormat(const Texture::TextureFormat& format)
{
    DXGI_FORMAT result = DXGI_FORMAT_UNKNOWN;

    switch (format.Components)
    {
    case Texture::Components::R:
        switch (format.Type)
        {
        case Texture::Type::Typeless:
            if (format.RedBits == 8)
            {
                result = DXGI_FORMAT_R8_TYPELESS;
            }
            else if (format.RedBits == 16)
            {
                result = DXGI_FORMAT_R16_TYPELESS;
            }
            else if (format.RedBits == 32)
            {
                result = DXGI_FORMAT_R32_TYPELESS;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose the best format based on the requested format.
                if (format.RedBits > 16)
                {
                    result = DXGI_FORMAT_R32_TYPELESS;
                }
                else if (format.RedBits > 8)
                {
                    result = DXGI_FORMAT_R16_TYPELESS;
                }
                else
                {
                    result = DXGI_FORMAT_R8_TYPELESS;
                }
            }
            break;
        case Texture::Type::UnsignedNormalized:
            if (format.RedBits == 1)
            {
                result = DXGI_FORMAT_R1_UNORM;
            }
            else if (format.RedBits == 8)
            {
                result = DXGI_FORMAT_R8_UNORM;
            }
            else if (format.RedBits == 16)
            {
                result = DXGI_FORMAT_R16_UNORM;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative to the requested format.
                if (format.RedBits > 8)
                {
                    result = DXGI_FORMAT_R16_UNORM;
                }
                else if (format.RedBits > 1)
                {
                    result = DXGI_FORMAT_R8_UNORM;
                }
                else
                {
                    result = DXGI_FORMAT_R1_UNORM;
                }
            }
            break;
        case Texture::Type::SignedNormalized:
            if (format.RedBits == 8)
            {
                result = DXGI_FORMAT_R8_SNORM;
            }
            else if (format.RedBits == 16)
            {
                result = DXGI_FORMAT_R16_SNORM;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 8)
                {
                    result = DXGI_FORMAT_R16_SNORM;
                }
                else
                {
                    result = DXGI_FORMAT_R8_SNORM;
                }
            }
            break;
        case Texture::Type::Float:
            if (format.RedBits == 16)
            {
                result = DXGI_FORMAT_R16_FLOAT;
            }
            else if (format.RedBits == 32)
            {
                result = DXGI_FORMAT_R32_FLOAT;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 16)
                {
                    result = DXGI_FORMAT_R32_FLOAT;
                }
                else
                {
                    result = DXGI_FORMAT_R16_FLOAT;
                }
            }
            break;
        case Texture::Type::UnsignedInteger:
            if (format.RedBits == 8)
            {
                result = DXGI_FORMAT_R8_UINT;
            }
            else if (format.RedBits == 16)
            {
                result = DXGI_FORMAT_R16_UINT;
            }
            else if (format.RedBits == 32)
            {
                result = DXGI_FORMAT_R32_UINT;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 16)
                {
                    result = DXGI_FORMAT_R32_UINT;
                }
                else if (format.RedBits > 8)
                {
                    result = DXGI_FORMAT_R16_UINT;
                }
                else
                {
                    result = DXGI_FORMAT_R8_UINT;
                }
            }
            break;
        case Texture::Type::SignedInteger:
            if (format.RedBits == 8)
            {
                result = DXGI_FORMAT_R8_SINT;
            }
            else if (format.RedBits == 16)
            {
                result = DXGI_FORMAT_R16_SINT;
            }
            else if (format.RedBits == 32)
            {
                result = DXGI_FORMAT_R32_SINT;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative
                if (format.RedBits > 16)
                {
                    result = DXGI_FORMAT_R32_SINT;
                }
                else if (format.RedBits > 8)
                {
                    result = DXGI_FORMAT_R16_SINT;
                }
                else
                {
                    result = DXGI_FORMAT_R8_SINT;
                }
            }
            break;
        default:
            //ReportTextureFormatError(format, "Unknown texture format.");
            break;
        }
        break;
    case Texture::Components::RG:
        switch (format.Type)
        {
        case Texture::Type::Typeless:
            if (format.RedBits == 8 && format.GreenBits == 8)
            {
                result = DXGI_FORMAT_R8G8_TYPELESS;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16)
            {
                result = DXGI_FORMAT_R16G16_TYPELESS;
            }
            else if (format.RedBits == 24 && format.GreenBits == 8)
            {
                result = DXGI_FORMAT_R24G8_TYPELESS;
            }
            else if (format.RedBits == 32 && format.GreenBits == 32)
            {
                result = DXGI_FORMAT_R32G32_TYPELESS;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose the best format based on the requested format.
                if (format.RedBits > 24 || format.GreenBits > 16)
                {
                    result = DXGI_FORMAT_R32G32_TYPELESS;
                }
                else if (format.RedBits > 16 && format.GreenBits <= 8)
                {
                    result = DXGI_FORMAT_R24G8_TYPELESS;
                }
                else if (format.RedBits > 8 || format.GreenBits > 8)
                {
                    result = DXGI_FORMAT_R16G16_TYPELESS;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8_TYPELESS;
                }
            }
            break;
        case Texture::Type::UnsignedNormalized:
            if (format.RedBits == 8 && format.GreenBits == 8)
            {
                result = DXGI_FORMAT_R8G8_UNORM;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16)
            {
                result = DXGI_FORMAT_R16G16_UNORM;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 8 || format.GreenBits > 8)
                {
                    result = DXGI_FORMAT_R16G16_UNORM;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8_UNORM;
                }
            }
            break;
        case Texture::Type::SignedNormalized:
            if (format.RedBits == 8 && format.GreenBits == 8)
            {
                result = DXGI_FORMAT_R8G8_SNORM;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16)
            {
                result = DXGI_FORMAT_R16G16_SNORM;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 8 || format.GreenBits > 8)
                {
                    result = DXGI_FORMAT_R16G16_SNORM;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8_SNORM;
                }
            }
            break;
        case Texture::Type::Float:
            if (format.RedBits == 16 && format.GreenBits == 16)
            {
                result = DXGI_FORMAT_R16G16_FLOAT;
            }
            else if (format.RedBits == 32 && format.GreenBits == 32)
            {
                result = DXGI_FORMAT_R32G32_FLOAT;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 16 || format.GreenBits > 16)
                {
                    result = DXGI_FORMAT_R32G32_FLOAT;
                }
                else
                {
                    result = DXGI_FORMAT_R16G16_FLOAT;
                }
            }
            break;
        case Texture::Type::UnsignedInteger:
            if (format.RedBits == 8 && format.GreenBits == 8)
            {
                result = DXGI_FORMAT_R8G8_UINT;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16)
            {
                result = DXGI_FORMAT_R16G16_UINT;
            }
            else if (format.RedBits == 32 && format.GreenBits == 32)
            {
                result = DXGI_FORMAT_R32G32_UINT;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 16 || format.GreenBits > 16)
                {
                    result = DXGI_FORMAT_R32G32_UINT;
                }
                else if (format.RedBits > 8 || format.GreenBits > 8)
                {
                    result = DXGI_FORMAT_R16G16_UINT;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8_UINT;
                }
            }
            break;
        case Texture::Type::SignedInteger:
            if (format.RedBits == 8 && format.GreenBits == 8)
            {
                result = DXGI_FORMAT_R8G8_SINT;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16)
            {
                result = DXGI_FORMAT_R16G16_SINT;
            }
            else if (format.RedBits == 32 && format.GreenBits == 32)
            {
                result = DXGI_FORMAT_R32G32_SINT;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 16 || format.GreenBits > 16)
                {
                    result = DXGI_FORMAT_R32G32_SINT;
                }
                else if (format.RedBits > 8 || format.GreenBits > 8)
                {
                    result = DXGI_FORMAT_R16G16_SINT;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8_SINT;
                }
            }
            break;
        default:
            //ReportTextureFormatError(format, "Unknown texture format.");
            break;
        }
        break;
    case Texture::Components::RGB:
        switch (format.Type)
        {
        case Texture::Type::Typeless:
            if (format.RedBits == 32 && format.GreenBits == 32 && format.BlueBits == 32)
            {
                result = DXGI_FORMAT_R32G32B32_TYPELESS;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // There is only 1 RGB typeless format
                result = DXGI_FORMAT_R32G32B32_TYPELESS;
            }
            break;
        case Texture::Type::Float:
            if (format.RedBits == 11 && format.GreenBits == 11 && format.BlueBits == 10)
            {
                result = DXGI_FORMAT_R11G11B10_FLOAT;
            }
            else if (format.RedBits == 32 && format.GreenBits == 32 && format.BlueBits == 32)
            {
                result = DXGI_FORMAT_R32G32B32_FLOAT;
            }
            else
            {
              //  ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 11 || format.GreenBits > 11 || format.BlueBits > 10)
                {
                    result = DXGI_FORMAT_R32G32B32_FLOAT;
                }
                else
                {
                    result = DXGI_FORMAT_R11G11B10_FLOAT;
                }
            }
            break;
        case Texture::Type::UnsignedInteger:
            if (format.RedBits == 32 && format.GreenBits == 32 && format.BlueBits == 32)
            {
                result = DXGI_FORMAT_R32G32B32_UINT;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // There is only 1 3-component UINT format.
                result = DXGI_FORMAT_R32G32B32_UINT;
            }
            break;
        case Texture::Type::SignedInteger:
            if (format.RedBits == 32 && format.GreenBits == 32 && format.BlueBits == 32)
            {
                result = DXGI_FORMAT_R32G32B32_SINT;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // There is only 1 3-component SINT format.
                result = DXGI_FORMAT_R32G32B32_SINT;
            }
            break;
        default:
           // ReportTextureFormatError(format, "Unsupported texture format.");
            // Try to choose a reasonable alternative
            switch (format.Type)
            {
            case Texture::Type::UnsignedNormalized:
                // This is a non-normalized format. May result in unintended behavior.
                result = DXGI_FORMAT_R32G32B32_UINT;
                break;
            case Texture::Type::SignedNormalized:
                // Non-normalized format. May result in unintended behavior.
                result = DXGI_FORMAT_R32G32B32_SINT;
                break;
            default:
                //ReportTextureFormatError(format, "Unknown texture format.");
                break;
            }
            break;
        }
        break;
    case Texture::Components::RGBA:
        switch (format.Type)
        {
        case Texture::Type::Typeless:
            if (format.RedBits == 8 && format.GreenBits == 8 && format.BlueBits == 8 && format.AlphaBits == 8)
            {
                result = DXGI_FORMAT_R8G8B8A8_TYPELESS;
            }
            else if (format.RedBits == 10 && format.GreenBits == 10 && format.BlueBits == 10 && format.AlphaBits == 2)
            {
                result = DXGI_FORMAT_R10G10B10A2_TYPELESS;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16 && format.BlueBits == 16 && format.AlphaBits == 16)
            {
                result = DXGI_FORMAT_R16G16B16A16_TYPELESS;
            }
            else if (format.RedBits == 32 && format.GreenBits == 32 && format.BlueBits == 32 && format.AlphaBits == 32)
            {
                result = DXGI_FORMAT_R32G32B32A32_TYPELESS;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose the best format based on the requested format.
                if (format.RedBits > 16 || format.GreenBits > 16 || format.BlueBits > 16 || format.AlphaBits > 16)
                {
                    result = DXGI_FORMAT_R32G32B32A32_TYPELESS;
                }
                else if ((format.RedBits > 10 || format.GreenBits > 10 || format.BlueBits > 10) && format.AlphaBits <= 2)
                {
                    result = DXGI_FORMAT_R10G10B10A2_TYPELESS;
                }
                else if (format.RedBits > 8 || format.GreenBits > 8 || format.BlueBits > 8 || format.AlphaBits > 8)
                {
                    result = DXGI_FORMAT_R16G16B16A16_TYPELESS;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8B8A8_TYPELESS;
                }
            }
            break;
        case Texture::Type::UnsignedNormalized:
            if (format.RedBits == 8 && format.GreenBits == 8 && format.BlueBits == 8 && format.AlphaBits == 8)
            {
                result = DXGI_FORMAT_R8G8B8A8_UNORM;
            }
            else if (format.RedBits == 10 && format.GreenBits == 10 && format.BlueBits == 10 && format.AlphaBits == 2)
            {
                result = DXGI_FORMAT_R10G10B10A2_UNORM;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16 && format.BlueBits == 16 && format.AlphaBits == 16)
            {
                result = DXGI_FORMAT_R16G16B16A16_UNORM;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 10 || format.GreenBits > 10 || format.BlueBits > 10 || format.AlphaBits > 8)
                {
                    result = DXGI_FORMAT_R16G16B16A16_UNORM;
                }
                else if ((format.RedBits > 8 || format.GreenBits > 8 || format.BlueBits > 8) && format.AlphaBits <= 2)
                {
                    result = DXGI_FORMAT_R10G10B10A2_UNORM;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8B8A8_UNORM;
                }
            }
            break;
        case Texture::Type::SignedNormalized:
            if (format.RedBits == 8 && format.GreenBits == 8 && format.BlueBits == 8 && format.AlphaBits == 8)
            {
                result = DXGI_FORMAT_R8G8B8A8_SNORM;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16 && format.BlueBits == 16 && format.AlphaBits == 16)
            {
                result = DXGI_FORMAT_R16G16B16A16_SNORM;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 8 || format.GreenBits > 8 || format.BlueBits > 8 || format.AlphaBits > 8)
                {
                    result = DXGI_FORMAT_R16G16B16A16_SNORM;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8B8A8_SNORM;
                }
            }
            break;
        case Texture::Type::Float:
            if (format.RedBits == 32 && format.GreenBits == 32 && format.BlueBits && format.AlphaBits == 32)
            {
                result = DXGI_FORMAT_R32G32B32A32_FLOAT;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16 && format.BlueBits == 16 && format.AlphaBits == 16)
            {
                result = DXGI_FORMAT_R16G16B16A16_FLOAT;
            }
            else
            {
                //ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 16 || format.GreenBits > 16 || format.BlueBits > 16 || format.AlphaBits > 16)
                {
                    result = DXGI_FORMAT_R32G32B32A32_FLOAT;
                }
                else
                {
                    result = DXGI_FORMAT_R16G16B16A16_FLOAT;
                }
            }
            break;
        case Texture::Type::UnsignedInteger:
            if (format.RedBits == 8 && format.GreenBits == 8 && format.BlueBits == 8 && format.AlphaBits == 8)
            {
                result = DXGI_FORMAT_R8G8B8A8_UINT;
            }
            else if (format.RedBits == 10 && format.GreenBits == 10 && format.BlueBits == 10 && format.AlphaBits == 2)
            {
                result = DXGI_FORMAT_R10G10B10A2_UINT;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16 && format.BlueBits == 16 && format.AlphaBits == 16)
            {
                result = DXGI_FORMAT_R16G16B16A16_UINT;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 10 || format.GreenBits > 10 || format.BlueBits > 10 || format.AlphaBits > 10)
                {
                    result = DXGI_FORMAT_R16G16B16A16_UINT;
                }
                else if ((format.RedBits > 8 || format.GreenBits > 8 || format.BlueBits > 8) && format.AlphaBits <= 2)
                {
                    result = DXGI_FORMAT_R10G10B10A2_UINT;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8B8A8_UINT;
                }
            }
            break;
        case Texture::Type::SignedInteger:
            if (format.RedBits == 8 && format.GreenBits == 8 && format.BlueBits == 8 && format.AlphaBits == 8)
            {
                result = DXGI_FORMAT_R8G8B8A8_SINT;
            }
            else if (format.RedBits == 16 && format.GreenBits == 16 && format.BlueBits == 16 && format.AlphaBits == 16)
            {
                result = DXGI_FORMAT_R16G16B16A16_SINT;
            }
            else if (format.RedBits == 32 && format.GreenBits == 32 && format.BlueBits == 32 && format.AlphaBits == 32)
            {
                result = DXGI_FORMAT_R32G32B32A32_SINT;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // Try to choose a reasonable alternative.
                if (format.RedBits > 16 || format.GreenBits > 16 || format.BlueBits > 16 || format.AlphaBits > 16)
                {
                    result = DXGI_FORMAT_R32G32B32A32_SINT;
                }
                if (format.RedBits > 8 || format.GreenBits > 8 || format.BlueBits > 8 || format.AlphaBits > 8)
                {
                    result = DXGI_FORMAT_R16G16B16A16_SINT;
                }
                else
                {
                    result = DXGI_FORMAT_R8G8B8A8_SINT;
                }
            }
            break;
        default:
            // ReportTextureFormatError(format, "Unknown texture format.");
            break;
        }
        break;
    case Texture::Components::Depth:
        switch (format.Type)
        {
        case Texture::Type::UnsignedNormalized:
            if (format.DepthBits == 16)
            {
                result = DXGI_FORMAT_D16_UNORM;
            }
            else
            {
                // ReportTextureFormatError(format, "Unsupported texture format.");
                // Only 1 format that could match.
                result = DXGI_FORMAT_D16_UNORM;
            }
            break;
        case Texture::Type::Float:
            if (format.DepthBits == 32)
            {
                result = DXGI_FORMAT_D32_FLOAT;
            }
            else
            {
               // ReportTextureFormatError(format, "Unsupported texture format.");
                // Only 1 format that could match.
                result = DXGI_FORMAT_D32_FLOAT;
            }
            break;
        default:
            // There are no SNORM, SINT, or UINT depth-only formats.
           // ReportTextureFormatError(format, "Unknown texture format.");
            break;
        }
        break;
    case Texture::Components::DepthStencil:
        // For Depth/Stencil formats, we'll ignore the type and try to deduce the format
        // based on the bit-depth values.
        if (format.DepthBits == 24 && format.StencilBits == 8)
        {
            result = DXGI_FORMAT_D24_UNORM_S8_UINT;
        }
        else if (format.DepthBits == 32 && format.StencilBits == 8)
        {
            result = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        }
        else
        {
           // ReportTextureFormatError(format, "Unsupported texture format.");
            // Try to choose a reasonable alternative...
            if (format.DepthBits > 24)
            {
                result = result = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
            }
            else
            {
                result = DXGI_FORMAT_D24_UNORM_S8_UINT;
            }
        }
        break;
    default:
       // ReportTextureFormatError(format, "Unknown texture format.");
        break;
    }

    return result;
}

Texture::TextureFormat ConvertFormat(DXGI_FORMAT format, uint8_t numSamples)
{
    Texture::TextureFormat result;
    result.NumSamples = numSamples;

    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 32;
        result.AlphaBits = 32;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::Float;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 32;
        result.AlphaBits = 32;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32B32A32_UINT:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 32;
        result.AlphaBits = 32;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32B32A32_SINT:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 32;
        result.AlphaBits = 32;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        result.Components = Texture::Components::RGB;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 32;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32B32_FLOAT:
        result.Components = Texture::Components::RGB;
        result.Type = Texture::Type::Float;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 32;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32B32_UINT:
        result.Components = Texture::Components::RGB;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 32;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32B32_SINT:
        result.Components = Texture::Components::RGB;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 32;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 16;
        result.AlphaBits = 16;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::Float;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 16;
        result.AlphaBits = 16;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16B16A16_UINT:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 16;
        result.AlphaBits = 16;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16B16A16_SNORM:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::SignedNormalized;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 16;
        result.AlphaBits = 16;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16B16A16_SINT:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 16;
        result.AlphaBits = 16;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32_TYPELESS:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32_FLOAT:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::Float;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32_UINT:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32G32_SINT:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 32;
        result.GreenBits = 32;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        result.Components = Texture::Components::DepthStencil;
        result.Type = Texture::Type::Float;
        result.RedBits = 0;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 32;
        result.StencilBits = 8;
        break;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 10;
        result.GreenBits = 10;
        result.BlueBits = 10;
        result.AlphaBits = 2;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R10G10B10A2_UNORM:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::UnsignedNormalized;
        result.RedBits = 10;
        result.GreenBits = 10;
        result.BlueBits = 10;
        result.AlphaBits = 2;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R10G10B10A2_UINT:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 10;
        result.GreenBits = 10;
        result.BlueBits = 10;
        result.AlphaBits = 2;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R11G11B10_FLOAT:
        result.Components = Texture::Components::RGB;
        result.Type = Texture::Type::Float;
        result.RedBits = 11;
        result.GreenBits = 11;
        result.BlueBits = 10;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 8;
        result.AlphaBits = 8;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::UnsignedNormalized;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 8;
        result.AlphaBits = 8;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8G8B8A8_UINT:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 8;
        result.AlphaBits = 8;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::SignedNormalized;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 8;
        result.AlphaBits = 8;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8G8B8A8_SINT:
        result.Components = Texture::Components::RGBA;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 8;
        result.AlphaBits = 8;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16_TYPELESS:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16_FLOAT:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::Float;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16_UNORM:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::UnsignedNormalized;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16_UINT:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16G16_SINT:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 16;
        result.GreenBits = 16;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32_TYPELESS:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 32;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_D32_FLOAT:
        result.Components = Texture::Components::Depth;
        result.Type = Texture::Type::Float;
        result.RedBits = 0;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 32;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32_FLOAT:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::Float;
        result.RedBits = 32;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32_UINT:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 32;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R32_SINT:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 32;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R24G8_TYPELESS:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 24;
        result.GreenBits = 8;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        result.Components = Texture::Components::DepthStencil;
        result.Type = Texture::Type::UnsignedNormalized;
        result.RedBits = 0;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 24;
        result.StencilBits = 8;
        break;
    case DXGI_FORMAT_R8G8_TYPELESS:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8G8_UNORM:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::UnsignedNormalized;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8G8_UINT:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8G8_SNORM:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::SignedNormalized;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8G8_SINT:
        result.Components = Texture::Components::RG;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 8;
        result.GreenBits = 8;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16_TYPELESS:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 16;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16_FLOAT:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::Float;
        result.RedBits = 16;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_D16_UNORM:
        result.Components = Texture::Components::Depth;
        result.Type = Texture::Type::UnsignedNormalized;
        result.RedBits = 0;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 16;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16_UNORM:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::UnsignedNormalized;
        result.RedBits = 16;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16_UINT:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 16;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16_SNORM:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::SignedNormalized;
        result.RedBits = 16;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R16_SINT:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 16;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8_TYPELESS:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::Typeless;
        result.RedBits = 8;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8_UNORM:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::UnsignedNormalized;
        result.RedBits = 8;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8_UINT:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::UnsignedInteger;
        result.RedBits = 8;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8_SNORM:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::SignedNormalized;
        result.RedBits = 8;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R8_SINT:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::SignedInteger;
        result.RedBits = 8;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    case DXGI_FORMAT_R1_UNORM:
        result.Components = Texture::Components::R;
        result.Type = Texture::Type::UnsignedNormalized;
        result.RedBits = 1;
        result.GreenBits = 0;
        result.BlueBits = 0;
        result.AlphaBits = 0;
        result.DepthBits = 0;
        result.StencilBits = 0;
        break;
    default:
        assert(false && "Unsupported DXGI format");
        break;
    }

    return result;
}

DXGI_FORMAT GetTextureFormat(DXGI_FORMAT format)
{
    DXGI_FORMAT result = format;

    switch (format)
    {
    case DXGI_FORMAT_D16_UNORM:
        result = DXGI_FORMAT_R16_TYPELESS;
        break;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        result = DXGI_FORMAT_R24G8_TYPELESS;
        break;
    case DXGI_FORMAT_D32_FLOAT:
        result = DXGI_FORMAT_R32_TYPELESS;
        break;
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        result = DXGI_FORMAT_R32G8X24_TYPELESS;
        break;
    }

    return result;
}

DXGI_FORMAT GetDSVFormat(DXGI_FORMAT format)
{
    DXGI_FORMAT result = format;

    switch (format)
    {
    case DXGI_FORMAT_R16_TYPELESS:
        result = DXGI_FORMAT_D16_UNORM;
        break;
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
        result = DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;
    case DXGI_FORMAT_R32_TYPELESS:
        result = DXGI_FORMAT_D32_FLOAT;
        break;
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        result = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        break;
    }

    return result;
}

DXGI_FORMAT GetSRVFormat(DXGI_FORMAT format)
{
    DXGI_FORMAT result = format;
    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        result = DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        result = DXGI_FORMAT_R32G32B32_FLOAT;
        break;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        result = DXGI_FORMAT_R16G16B16A16_UNORM;
        break;
    case DXGI_FORMAT_R32G32_TYPELESS:
        result = DXGI_FORMAT_R32G32_FLOAT;
        break;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        result = DXGI_FORMAT_R10G10B10A2_UNORM;
        break;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        result = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
    case DXGI_FORMAT_R16G16_TYPELESS:
        result = DXGI_FORMAT_R16G16_UNORM;
        break;
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_D16_UNORM:
        result = DXGI_FORMAT_R16_UNORM;
        break;
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        result = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        break;
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
        result = DXGI_FORMAT_R32_FLOAT;
        break;
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        result = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        break;
    case DXGI_FORMAT_R8G8_TYPELESS:
        result = DXGI_FORMAT_R8G8_UNORM;
        break;
    case DXGI_FORMAT_R8_TYPELESS:
        result = DXGI_FORMAT_R8_UNORM;
        break;
    }

    return result;
}

DXGI_FORMAT GetRTVFormat(DXGI_FORMAT format)
{
    DXGI_FORMAT result = format;

    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        result = DXGI_FORMAT_R32G32B32A32_FLOAT;
        break;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        result = DXGI_FORMAT_R32G32B32_FLOAT;
        break;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        result = DXGI_FORMAT_R16G16B16A16_UNORM;
        break;
    case DXGI_FORMAT_R32G32_TYPELESS:
        result = DXGI_FORMAT_R32G32_FLOAT;
        break;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
        result = DXGI_FORMAT_R10G10B10A2_UNORM;
        break;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        result = DXGI_FORMAT_R8G8B8A8_UNORM;
        break;
    case DXGI_FORMAT_R16G16_TYPELESS:
        result = DXGI_FORMAT_R16G16_UNORM;
        break;
    case DXGI_FORMAT_R8G8_TYPELESS:
        result = DXGI_FORMAT_R8G8_UNORM;
        break;
    case DXGI_FORMAT_R32_TYPELESS:
        result = DXGI_FORMAT_R32_FLOAT;
        break;
    case DXGI_FORMAT_R8_TYPELESS:
        result = DXGI_FORMAT_R8_UNORM;
        break;
    }

    return result;
}

DXGI_FORMAT GetUAVFormat(DXGI_FORMAT format)
{
    return GetRTVFormat(format);
}

uint8_t GetBPP(DXGI_FORMAT format)
{
    uint8_t bpp = 0;

    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        bpp = 128;
        break;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        bpp = 96;
        break;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
        bpp = 64;
        break;
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
        bpp = 64;
        break;
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        bpp = 64;
        break;
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
        bpp = 32;
        break;
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
        bpp = 32;
        break;
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
        bpp = 32;
        break;
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
        bpp = 32;
        break;
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
        bpp = 32;
        break;
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
        bpp = 16;
        break;
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
        bpp = 16;
        break;
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        bpp = 8;
        break;
    case DXGI_FORMAT_R1_UNORM:
        bpp = 1;
        break;
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
        bpp = 32;
        break;
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
        bpp = 32;
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        bpp = 16;
        break;
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        bpp = 16;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
        bpp = 32;
        break;
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
        bpp = 32;
        break;
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        bpp = 32;
        break;
    default:
        assert(false && "Unsupported texture format");
        break;
    }

    return bpp;
}

DXGI_SAMPLE_DESC GetSupportedSampleCount(DXGI_FORMAT format, uint8_t numSamples)
{
    DXGI_SAMPLE_DESC sampleDesc;

    UINT sampleCount = 1;
    UINT qualityLevels = 0;

    while (sampleCount <= numSamples && SUCCEEDED(device->CheckMultisampleQualityLevels(format, sampleCount, &qualityLevels)) && qualityLevels > 0)
    {
        // That works...
        sampleDesc.Count = sampleCount;
        sampleDesc.Quality = qualityLevels - 1;

        // But can we do better?
        sampleCount = sampleCount * 2;
    }

    return sampleDesc;
}

Ref<D3D11Texture> D3D11Texture::CreateTexture2D(uint16_t width, uint16_t height, uint16_t slices, const TextureFormat& format, CPUAccess cpuAccess, bool uav)
{
	Ref<D3D11Texture> texture = CreateRef<D3D11Texture>();

	texture->m_TextureFormat = format;
	texture->m_CPUAccess = cpuAccess;
	texture->m_IsTransparent = true;

	texture->m_NumSlices = Math::Max<uint16_t>(slices, 1);

	texture->m_TextureDimension = Dimension::Texture2D;
	if (texture->m_NumSlices > 1)
		texture->m_TextureDimension = Dimension::Texture2DArray;

	DXGI_FORMAT dxgiFormat = ConvertFormat(format);

    texture->m_SampleDesc = GetSupportedSampleCount(dxgiFormat, format.NumSamples);

    texture->m_TextureFormat = ConvertFormat(dxgiFormat, format.NumSamples);

    // Convert Depth/Stencil formats to typeless texture resource formats
    texture->m_TextureResourceFormat = GetTextureFormat(dxgiFormat);
    // Convert typeless formats to Depth/Stencil view formats
    texture->m_DepthStencilViewFormat = GetDSVFormat(dxgiFormat);
    // Convert depth/stencil and typeless formats to Shader Resource View formats
    texture->m_ShaderResourceViewFormat = GetSRVFormat(dxgiFormat);
    // Convert typeless formats to Render Target View formats
    texture->m_RenderTargetViewFormat = GetRTVFormat(dxgiFormat);
    // Convert typeless format to Unordered Access View formats.
    texture->m_UnorderedAccessViewFormat = GetUAVFormat(dxgiFormat);

    texture->m_BPP = LevEngine::GetBPP(texture->m_TextureResourceFormat);

    // Query for texture format support.
    if (FAILED(device->CheckFormatSupport(texture->m_TextureResourceFormat, &texture->m_TextureResourceFormatSupport)))
    {
        assert(false && "Failed to query texture resource format support");
    }
    if (FAILED(device->CheckFormatSupport(texture->m_DepthStencilViewFormat, &texture->m_DepthStencilViewFormatSupport)))
    {
        assert(false && "Failed to query depth/stencil format support");
    }
    if (FAILED(device->CheckFormatSupport(texture->m_ShaderResourceViewFormat, &texture->m_ShaderResourceViewFormatSupport)))
    {
        assert(false && "Failed to query shader resource format support");
    }
    if (FAILED(device->CheckFormatSupport(texture->m_RenderTargetViewFormat, &texture->m_RenderTargetViewFormatSupport)))
    {
        assert(false && "Failed to query render target format support");
    }
    if (FAILED(device->CheckFormatSupport(texture->m_UnorderedAccessViewFormat, &texture->m_UnorderedAccessViewFormatSupport)))
    {
        assert(false && "Failed to query uav format support");
    }
    if ((texture->m_TextureResourceFormatSupport & D3D11_FORMAT_SUPPORT_TEXTURE2D) == 0)
    {
        assert(false && "Unsupported texture format for 2D textures");
        //ReportTextureFormatError(m_TextureFormat, "Unsupported texture format for 2D textures.");
    }
    // Can the texture be dynamically modified on the CPU?
    texture->m_Dynamic = (int)texture->m_CPUAccess != 0 && (texture->m_TextureResourceFormatSupport & D3D11_FORMAT_SUPPORT_CPU_LOCKABLE) != 0;
    // Can mipmaps be automatically generated for this texture format?
    texture->m_GenerateMipMaps = !texture->m_Dynamic && (texture->m_ShaderResourceViewFormatSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN) != 0;
    // Are UAVs supported?
    texture->m_UAV = uav && (texture->m_UnorderedAccessViewFormatSupport & D3D11_FORMAT_SUPPORT_SHADER_LOAD) != 0;

    texture->Resize(width, height);

	return texture;
}

D3D11Texture::D3D11Texture(const std::string& path)
	: m_Path(path)
{
	// Load image

	stbi_set_flip_vertically_on_load(1);

	int width, height, channels;

	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

	if (!data) return;

    if (channels == 4)
    {
        m_TextureResourceFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    }
	else if (channels == 3)
    {
        m_TextureResourceFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    }
    else
    {
        assert(false && "Unsupported number of channels");
    }

    if (FAILED(device->CheckFormatSupport(m_TextureResourceFormat, &m_TextureResourceFormatSupport)))
    {
        assert(false && "Failed to query format support");
    }
    if ((m_TextureResourceFormatSupport & D3D11_FORMAT_SUPPORT_TEXTURE2D) == 0)
    {
        assert(false && "Unsupported texture format for 2D textures");
        //ReportTextureFormatError(m_TextureFormat, "Unsupported texture format for 2D textures.");
        return;
    }

	// Texture

    m_Pitch = width * 4;

    m_IsLoaded = true;
    m_Width = width;
    m_Height = height;
    m_TextureDimension = Dimension::Texture2D;
    m_NumSlices = 1;
    m_SampleDesc = GetSupportedSampleCount(m_TextureResourceFormat, 1);

    m_ShaderResourceViewFormat = m_RenderTargetViewFormat = m_TextureResourceFormat;

    m_ShaderResourceViewFormatSupport = m_RenderTargetViewFormatSupport = m_TextureResourceFormatSupport;

    m_GenerateMipMaps = !m_Dynamic && (m_ShaderResourceViewFormatSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN) != 0;

	D3D11_TEXTURE2D_DESC textureDesc = {};

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = m_GenerateMipMaps ? 0 : 1;
	textureDesc.ArraySize = m_NumSlices;
	textureDesc.Format = m_TextureResourceFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    if ((m_ShaderResourceViewFormatSupport & D3D11_FORMAT_SUPPORT_SHADER_SAMPLE) != 0)
    {
        textureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    }
    if ((m_RenderTargetViewFormatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET) != 0)
    {
        textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
    }

    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = m_GenerateMipMaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	D3D11_SUBRESOURCE_DATA subresourceData = {};

	subresourceData.pSysMem = data;
	subresourceData.SysMemPitch = m_Pitch;
    subresourceData.SysMemSlicePitch = 0;

	auto result = device->CreateTexture2D(
		&textureDesc,
        m_GenerateMipMaps ? nullptr : &subresourceData,
		&m_Texture2D
	);

	assert(SUCCEEDED(result));

	//Shader resource view

    D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;

    resourceViewDesc.Format = m_ShaderResourceViewFormat;
    resourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
    resourceViewDesc.Texture2D.MipLevels = m_GenerateMipMaps ? -1 : 1;
    resourceViewDesc.Texture2D.MostDetailedMip = 0;


	result = device->CreateShaderResourceView(m_Texture2D,
		&resourceViewDesc,
		&m_ShaderResourceView
	);

	assert(SUCCEEDED(result));

    if (m_GenerateMipMaps)
    {
        context->UpdateSubresource(m_Texture2D, 0, nullptr, data, m_Pitch, 0);
        context->GenerateMips(m_ShaderResourceView);
    }

    m_IsDirty = false;

	// Sampler

	D3D11_SAMPLER_DESC ImageSamplerDesc = {};

	ImageSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ImageSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	ImageSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	ImageSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	ImageSamplerDesc.MipLODBias = 0.0f;
	ImageSamplerDesc.MaxAnisotropy = 1;
	ImageSamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	ImageSamplerDesc.BorderColor[0] = 1.0f;
	ImageSamplerDesc.BorderColor[1] = 1.0f;
	ImageSamplerDesc.BorderColor[2] = 1.0f;
	ImageSamplerDesc.BorderColor[3] = 1.0f;
	ImageSamplerDesc.MinLOD = -FLT_MAX;
	ImageSamplerDesc.MaxLOD = FLT_MAX;

	result = device->CreateSamplerState(&ImageSamplerDesc,
		&m_SamplerState);

	assert(SUCCEEDED(result));

    stbi_image_free(data);
}

D3D11Texture::~D3D11Texture()
{
    if (m_Texture1D) m_Texture1D->Release();
    if (m_Texture2D) m_Texture2D->Release();
    if (m_Texture3D) m_Texture3D->Release();

    if (m_ShaderResourceView) m_ShaderResourceView->Release();
    if (m_RenderTargetView) m_RenderTargetView->Release();
    if (m_DepthStencilView) m_DepthStencilView->Release();
    if (m_UnorderedAccessView) m_UnorderedAccessView->Release();

    if (m_SamplerState) m_SamplerState->Release();
}

void D3D11Texture::Bind(const uint32_t slot) const
{
	context->PSSetShaderResources(slot, 1, &m_ShaderResourceView);
    if (m_SamplerState)
		context->PSSetSamplers(slot, 1, &m_SamplerState);

    context->CSSetShaderResources(slot, 1, &m_ShaderResourceView);
    if (m_SamplerState)
        context->CSSetSamplers(slot, 1, &m_SamplerState);
}

void D3D11Texture::Clear(ClearFlags clearFlags, const Vector4& color, float depth, uint8_t stencil)
{
	if (m_RenderTargetView && ((int)clearFlags & (int)ClearFlags::Color) != 0)
	{
		context->ClearRenderTargetView(m_RenderTargetView, &color.x);
	}

	{
		UINT flags = 0;
		flags |= ((int)clearFlags & (int)ClearFlags::Depth) != 0 ? D3D11_CLEAR_DEPTH : 0;
		flags |= ((int)clearFlags & (int)ClearFlags::Stencil) != 0 ? D3D11_CLEAR_STENCIL : 0;
		if (m_DepthStencilView && flags > 0)
		{
			context->ClearDepthStencilView(m_DepthStencilView, flags, depth, stencil);
		}
	}
}

ID3D11Resource* D3D11Texture::GetTextureResource() const
{
	ID3D11Resource* resource = nullptr;

	switch (m_TextureDimension)
	{
	case Dimension::Texture1D:
	case Dimension::Texture1DArray:
		resource = m_Texture1D;
		break;
	case Dimension::Texture2D:
	case Dimension::Texture2DArray:
		resource = m_Texture2D;
		break;
	case Dimension::Texture3D:
	case Dimension::TextureCube:
		resource = m_Texture3D;
		break;
	default:
		assert(false && "Unknown texture dimension.");
		break;
	}

	return resource;
}

void D3D11Texture::Resize2D(uint16_t width, uint16_t height)
{
    if (m_Width == width && m_Height == height) return;

    // Release resource before resizing
    if (m_Texture2D) m_Texture2D->Release(); m_Texture2D = nullptr;
    if (m_RenderTargetView) m_RenderTargetView->Release(); m_RenderTargetView = nullptr;
    if (m_DepthStencilView) m_DepthStencilView->Release(); m_DepthStencilView = nullptr;
    if (m_ShaderResourceView) m_ShaderResourceView->Release(); m_ShaderResourceView = nullptr;
    if (m_UnorderedAccessView) m_UnorderedAccessView->Release(); m_UnorderedAccessView = nullptr;

    m_Width = Math::Max<uint16_t>(width, 1);
    m_Height = Math::Max<uint16_t>(height, 1);

    // Create texture with the dimensions specified.
    D3D11_TEXTURE2D_DESC textureDesc = { 0 };

    textureDesc.ArraySize = m_NumSlices;
    textureDesc.Format = m_TextureResourceFormat;
    textureDesc.SampleDesc = m_SampleDesc;

    textureDesc.Width = m_Width;
    textureDesc.Height = m_Height;
    textureDesc.MipLevels = 1;

    if (((int)m_CPUAccess & (int)CPUAccess::Read) != 0)
    {
	    textureDesc.Usage = D3D11_USAGE_STAGING;
	    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
    }
    else if (((int)m_CPUAccess & (int)CPUAccess::Write) != 0)
    {
	    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }
    else
    {
	    textureDesc.Usage = D3D11_USAGE_DEFAULT;
	    textureDesc.CPUAccessFlags = 0;
    }

    if (!m_UAV && !m_Dynamic && (m_DepthStencilViewFormatSupport & D3D11_FORMAT_SUPPORT_DEPTH_STENCIL) != 0)
    {
	    textureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
    }
    if (!m_Dynamic && (m_RenderTargetViewFormatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET) != 0)
    {
	    textureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
    }
    if (((int)m_CPUAccess & (int)CPUAccess::Read) == 0)
    {
	    textureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    }
    if (m_UAV && !m_Dynamic)
    {
	    textureDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    }

    textureDesc.MiscFlags = m_GenerateMipMaps ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

    if (FAILED(device->CreateTexture2D(&textureDesc, nullptr, &m_Texture2D)))
    {
	    assert(false && "Failed to create texture");
	    return;
    }

    if ((textureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL) != 0)
    {
	    // Create the depth/stencil view for the texture.
	    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	    depthStencilViewDesc.Format = m_DepthStencilViewFormat;
	    depthStencilViewDesc.Flags = 0;

	    if (m_NumSlices > 1)
	    {
		    if (m_SampleDesc.Count > 1)
		    {
			    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
			    depthStencilViewDesc.Texture2DMSArray.FirstArraySlice = 0;
			    depthStencilViewDesc.Texture2DMSArray.ArraySize = m_NumSlices;
		    }
		    else
		    {
			    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			    depthStencilViewDesc.Texture2DArray.MipSlice = 0;
			    depthStencilViewDesc.Texture2DArray.FirstArraySlice = 0;
			    depthStencilViewDesc.Texture2DArray.ArraySize = m_NumSlices;
		    }
	    }
	    else
	    {
		    if (m_SampleDesc.Count > 1)
		    {
			    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		    }
		    else
		    {
			    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			    depthStencilViewDesc.Texture2D.MipSlice = 0;
		    }
	    }

	    if (FAILED(device->CreateDepthStencilView(m_Texture2D, &depthStencilViewDesc, &m_DepthStencilView)))
	    {
		    assert(false && "Failed to create depth/stencil view");
	    }
    }

    if ((textureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE) != 0)
    {
	    // Create a Shader resource view for the texture.
	    D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc;
	    resourceViewDesc.Format = m_ShaderResourceViewFormat;

	    if (m_NumSlices > 1)
	    {
		    if (m_SampleDesc.Count > 1)
		    {
			    resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
			    resourceViewDesc.Texture2DMSArray.FirstArraySlice = 0;
			    resourceViewDesc.Texture2DMSArray.ArraySize = m_NumSlices;
		    }
		    else
		    {
			    resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			    resourceViewDesc.Texture2DArray.FirstArraySlice = 0;
			    resourceViewDesc.Texture2DArray.ArraySize = m_NumSlices;
			    resourceViewDesc.Texture2DArray.MipLevels = m_GenerateMipMaps ? -1 : 1;
			    resourceViewDesc.Texture2DArray.MostDetailedMip = 0;
		    }
	    }
	    else
	    {
		    if (m_SampleDesc.Count > 1)
		    {
			    resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		    }
		    else
		    {
			    resourceViewDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
			    resourceViewDesc.Texture2D.MipLevels = m_GenerateMipMaps ? -1 : 1;
			    resourceViewDesc.Texture2D.MostDetailedMip = 0;
		    }
	    }

	    if (FAILED(device->CreateShaderResourceView(m_Texture2D, &resourceViewDesc, &m_ShaderResourceView)))
	    {
		    assert(false && "Failed to create texture resource view.");
	    }
	    else if (m_GenerateMipMaps)
	    {
		    context->GenerateMips(m_ShaderResourceView);
	    }
    }

    if ((textureDesc.BindFlags & D3D11_BIND_RENDER_TARGET) != 0)
    {
	    // Create the render target view for the texture.
	    D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	    renderTargetViewDesc.Format = m_RenderTargetViewFormat;

	    if (m_NumSlices > 1)
	    {
		    if (m_SampleDesc.Count > 1)
		    {
			    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			    renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
			    renderTargetViewDesc.Texture2DArray.ArraySize = m_NumSlices;

		    }
		    else
		    {
			    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			    renderTargetViewDesc.Texture2DArray.MipSlice = 0;
			    renderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
			    renderTargetViewDesc.Texture2DArray.ArraySize = m_NumSlices;
		    }
	    }
	    else
	    {
		    if (m_SampleDesc.Count > 1)
		    {
			    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		    }
		    else
		    {
			    renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			    renderTargetViewDesc.Texture2D.MipSlice = 0;
		    }
	    }

	    if (FAILED(device->CreateRenderTargetView(m_Texture2D, &renderTargetViewDesc, &m_RenderTargetView)))
	    {
		    assert(false && "Failed to create render target view.");
	    }
    }

    if ((textureDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) != 0)
    {
	    // UAVs cannot be multi sampled.
	    assert(m_SampleDesc.Count == 1);

	    // Create a Shader resource view for the texture.
	    D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc;
	    unorderedAccessViewDesc.Format = m_UnorderedAccessViewFormat;

	    if (m_NumSlices > 1)
	    {
		    unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
		    unorderedAccessViewDesc.Texture2DArray.MipSlice = 0;
		    unorderedAccessViewDesc.Texture2DArray.FirstArraySlice = 0;
		    unorderedAccessViewDesc.Texture2DArray.ArraySize = m_NumSlices;
	    }
	    else
	    {
		    unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		    unorderedAccessViewDesc.Texture2D.MipSlice = 0;
	    }

	    if (FAILED(device->CreateUnorderedAccessView(m_Texture2D, &unorderedAccessViewDesc, &m_UnorderedAccessView)))
	    {
		    assert(false && "Failed to create unordered access view.");
	    }
    }

    assert(m_BPP > 0 && m_BPP % 8 == 0);
    m_Buffer.resize(width * height * (m_BPP / 8));
}


void D3D11Texture::Resize(uint16_t width, uint16_t height, uint16_t depth)
{
	switch (m_TextureDimension)
	{
	case Dimension::Texture1D:
	case Dimension::Texture1DArray:
		//Resize1D(width); //TODO: Texture1D Support
		break;
	case Dimension::Texture2D:
	case Dimension::Texture2DArray:
		Resize2D(width, height);
		break;
	case Dimension::Texture3D:
		//Resize3D(width, height, depth); //TODO: Texture3D Support
		break;
	case Dimension::TextureCube:
		//ResizeCube(width); //TODO: TextureCube Support
		break;
	default:
		assert(false && "Unknown texture dimension.");
		break;
	}
}

void D3D11Texture::Copy(Ref<Texture> other)
{
    std::shared_ptr<D3D11Texture> srcTexture = std::dynamic_pointer_cast<D3D11Texture>(other);

    if (srcTexture && srcTexture.get() != this)
    {
        if (m_TextureDimension == srcTexture->m_TextureDimension &&
            m_Width == srcTexture->m_Width &&
            m_Height == srcTexture->m_Height)
        {
            switch (m_TextureDimension)
            {
            case Dimension::Texture1D:
            case Dimension::Texture1DArray:
                context->CopyResource(m_Texture1D, srcTexture->m_Texture1D);
                break;
            case Dimension::Texture2D:
            case Dimension::Texture2DArray:
                context->CopyResource(m_Texture2D, srcTexture->m_Texture2D);
                break;
            case Dimension::Texture3D:
            case Dimension::TextureCube:
                context->CopyResource(m_Texture3D, srcTexture->m_Texture3D);
                break;
            }
        }
        else
        {
            assert(false && "Incompatible source texture");
        }
    }

    if (((int)m_CPUAccess & (int)CPUAccess::Read) != 0 && m_Texture2D)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;

        // Copy the texture data from the texture resource
        if (FAILED(context->Map(m_Texture2D, 0, D3D11_MAP_READ, 0, &mappedResource)))
	        assert(false && "Failed to map texture resource for reading.");

        memcpy_s(m_Buffer.data(), m_Buffer.size(), mappedResource.pData, m_Buffer.size());

        context->Unmap(m_Texture2D, 0);
    }
}

void D3D11Texture::Unbind(uint32_t slot) const
{
    ID3D11ShaderResourceView* srv[] = { nullptr };
    ID3D11UnorderedAccessView* uav[] = { nullptr };

    context->VSSetShaderResources(slot, 1, srv);
    context->HSSetShaderResources(slot, 1, srv);
    context->DSSetShaderResources(slot, 1, srv);
    context->GSSetShaderResources(slot, 1, srv);
    context->PSSetShaderResources(slot, 1, srv);
    context->CSSetShaderResources(slot, 1, srv);
    context->CSSetUnorderedAccessViews(slot, 1, uav, nullptr);
	context->CSSetSamplers(slot, 0, nullptr);
}

void D3D11Texture::GenerateMipMaps()
{
	if (m_GenerateMipMaps && m_ShaderResourceView)
		context->GenerateMips(m_ShaderResourceView);
}
}