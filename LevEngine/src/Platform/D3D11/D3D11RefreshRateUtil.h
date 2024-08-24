#pragma once
#include <wrl/client.h>

namespace LevEngine
{
    static DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync)
    {
        DXGI_RATIONAL refreshRate = {0, 1};
        if (!vsync) return refreshRate;

        Microsoft::WRL::ComPtr<IDXGIFactory> factory;
        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        Microsoft::WRL::ComPtr<IDXGIOutput> adapterOutput;

        // Create a DirectX graphics interface factory.
        if (FAILED(CreateDXGIFactory( __uuidof( IDXGIFactory ), &factory )))
        {
            LEV_ASSERT("Failed to create DXGIFactory");
        }

        if (FAILED(factory->EnumAdapters( 0, &adapter )))
        {
            LEV_ASSERT("Failed to enumerate adapters.");
        }

        if (FAILED(adapter->EnumOutputs( 0, &adapterOutput )))
        {
            LEV_ASSERT("Failed to enumerate adapter outputs.");
        }

        UINT numDisplayModes;
        if (FAILED(
            adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes,
                NULL )))
        {
            LEV_ASSERT("Failed to query display modes.");
        }

        DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numDisplayModes];
        assert(displayModeList);

        if (FAILED(
            adapterOutput->GetDisplayModeList( DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes,
                displayModeList )))
        {
            LEV_ASSERT("Failed to query dispaly mode list.");
        }

        // Now store the refresh rate of the monitor that matches the width and height of the requested screen.
        for (UINT i = 0; i < numDisplayModes; ++i)
        {
            if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
            {
                refreshRate = displayModeList[i].RefreshRate;
            }
        }

        delete[] displayModeList;

        return refreshRate;
    }
}
