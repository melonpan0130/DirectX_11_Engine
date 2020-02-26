#pragma once
#include "AdapterRender.h"

class Graphics
{
public:
	bool Initialize(HWND hwnd, int width, int height);
	void RenderFrame();
private:
	bool InitializeDirectX(HWND hwnd, int width, int height);

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr <ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr <IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> renderTargetView;
};