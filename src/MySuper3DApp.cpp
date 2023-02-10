// MySuper3DApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>

#include "D3D11IndexBuffer.h"
#include "D3D11Shader.h"
#include "D3D11VertexBuffer.h"


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

ID3D11DeviceContext* context;
HWND hWnd;
Microsoft::WRL::ComPtr<ID3D11Device> device;

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;

		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}


void InitWindow(HWND& hWnd, LPCWSTR applicationName, HINSTANCE hInstance, int screenWidth, int screenHeight)
{
	WNDCLASSEX wc{};

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	constexpr auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	const auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	const auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
	                      dwStyle,
	                      posX, posY,
	                      windowRect.right - windowRect.left,
	                      windowRect.bottom - windowRect.top,
	                      nullptr, nullptr, hInstance, nullptr);

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	ShowCursor(true);
}

bool InitD3D(const int screenWidth, const int screenHeight, HWND hWnd, Microsoft::WRL::ComPtr<ID3D11Device>& device, ID3D11DeviceContext*& context, IDXGISwapChain*& swapChain)
{
	constexpr D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = screenWidth;
	swapDesc.BufferDesc.Height = screenHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	const auto res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&device,
		nullptr,
		&context);

	return SUCCEEDED(res);
}

D3D11VertexBuffer* CreateVertexBuffer()
{
	DirectX::XMFLOAT4 points[8] =
	{
		DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f),		DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, -0.5f, 0.5f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),

		DirectX::XMFLOAT4(0.5f, -0.5f, 0.5f, 1.0f),		DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.5f, 0.5f, 0.5f, 1.0f),		DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};

	constexpr auto size = sizeof(DirectX::XMFLOAT4) * std::size(points);
	auto* vertexBuffer = new D3D11VertexBuffer(reinterpret_cast<float*>(points), size);
	return vertexBuffer;
}

D3D11IndexBuffer* CreateIndexBuffer()
{
	uint32_t indices[] = { 0,1,2, 1,0,3 };
	auto* indexBuffer = new D3D11IndexBuffer(indices, std::size(indices));
	return indexBuffer;
}

bool CreateRastState(ID3D11RasterizerState*& rastState)
{
	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	auto res = device->CreateRasterizerState(&rastDesc, &rastState);

	return SUCCEEDED(res);
}

int main()
{
	const LPCWSTR applicationName = L"My3DApp";
	const HINSTANCE hInstance = GetModuleHandle(nullptr);
	constexpr auto screenWidth = 800;
	constexpr auto screenHeight = 800;

	InitWindow(hWnd, applicationName, hInstance, screenWidth, screenHeight);

	IDXGISwapChain* swapChain;

	if (!InitD3D(screenWidth, screenHeight, hWnd, device, context, swapChain))
	{
		std::cout << "D3D init is failed. Exiting...";
		return 2;
	}

	ID3D11Texture2D* backTex;
	ID3D11RenderTargetView* rtv;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backTex));	// __uuidof(ID3D11Texture2D)
	device->CreateRenderTargetView(backTex, nullptr, &rtv);

	D3D11Shader shader{"./Shaders/MyVeryFirstShader.hlsl"};

	auto* vb = CreateVertexBuffer();

	const BufferLayout layout{
	{ ShaderDataType::Float4, "POSITION" },
	{ ShaderDataType::Float4, "COLOR" },
	};
	shader.SetLayout(layout);
	vb->SetLayout(layout);

	D3D11IndexBuffer* ib = CreateIndexBuffer();

	ID3D11RasterizerState* rastState;
	if (!CreateRastState(rastState))
		return 5;

	context->RSSetState(rastState);

	std::chrono::time_point<std::chrono::steady_clock> PrevTime = std::chrono::steady_clock::now();
	float totalTime = 0;
	unsigned int frameCount = 0;

	MSG msg = {};
	bool isExitRequested = false;

	constexpr UINT strides[] = { 32 };
	constexpr UINT offsets[] = { 0 };

	while (!isExitRequested) {
		// Handle the windows messages.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			isExitRequested = true;
		}

		context->ClearState();

		context->RSSetState(rastState);

		D3D11_VIEWPORT viewport = {};
		viewport.Width = static_cast<float>(screenWidth);
		viewport.Height = static_cast<float>(screenHeight);
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1.0f;

		context->RSSetViewports(1, &viewport);
		shader.Bind();
		context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ib->Bind();
		vb->Bind();

		auto curTime = std::chrono::steady_clock::now();
		const float deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
		PrevTime = curTime;

		totalTime += deltaTime;
		frameCount++;

		if (totalTime > 1.0f) 
		{
			const float fps = frameCount / totalTime;

			totalTime -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(hWnd, text);

			frameCount = 0;
		}

		context->OMSetRenderTargets(1, &rtv, nullptr);

		const float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
		context->ClearRenderTargetView(rtv, color);

		context->DrawIndexed(6, 0, 0);

		context->OMSetRenderTargets(0, nullptr, nullptr);

		swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
	}

	std::cout << "Hello World!\n";
}
