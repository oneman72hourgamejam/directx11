#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "DxErr.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

#include <windows.h>
#include <d3d11.h>
#include <D3DX11.h>
#include <D3DX10.h>
#include <DxErr.h>
#include <xnamath.h>

LPCTSTR WndClassName = "window";
HWND hwnd = NULL;

const int Width = 800;
const int Height = 600;

IDXGISwapChain* SwapChain;
ID3D11Device* d3d11Device;
ID3D11DeviceContext* d3d11DevCon;
ID3D11RenderTargetView* renderTargetView;

float red = 0.0f;
float green = 0.0f;
float blue = 0.0f;
int colormodr = 1;
int colormodg = 1;
int colormodb = 1;

bool InitializeDirect3d11App(HINSTANCE hInstance);
void ReleaseObjects();
bool InitScene();
void UpdateScene();
void DrawScene();

bool InitializeWindow(HINSTANCE hInstance, 
	int ShowWnd,
	int width, int height,
	bool windowed);

int messageloop();

LRESULT CALLBACK WndProc(HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)
{
	if (!InitializeWindow(hInstance, nShowCmd, Width, Height, true))
	{
		MessageBox(0, "Init Failed", "Error", MB_OK);
		return 0;
	}

	if (!InitializeDirect3d11App(hInstance))
	{
		MessageBox(0, "Direct Failed", "Error", MB_OK);
		return 0;
	}

	if (!InitScene())
	{
		MessageBox(0, "Scene Failed", "Error", MB_OK);
		return 0;
	}

	messageloop();

	ReleaseObjects();

	return 0;
}

bool InitializeWindow(HINSTANCE hInstance,
	int ShowWnd,
	int width, int height,
	bool windowed)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WndClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Error registering class",
			"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	hwnd = CreateWindowEx(NULL, WndClassName,
		"Window", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		width, height,
		NULL, NULL, hInstance, NULL);

	if (!hwnd)
	{
		MessageBox(NULL, "Error creating window",
			"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(hwnd, ShowWnd);

	UpdateWindow(hwnd);

	return true;
}

bool InitializeDirect3d11App(HINSTANCE hInstance)
{
	HRESULT hr;

	DXGI_MODE_DESC bufferDesc;

	ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

	bufferDesc.Width = Width;
	bufferDesc.Height = Height;
	bufferDesc.RefreshRate.Numerator = 60;
	bufferDesc.RefreshRate.Denominator = 1;
	bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
		NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &swapChainDesc,
		&SwapChain, &d3d11Device, NULL, &d3d11DevCon);

	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr), "SwapChain", MB_OK);
		return 0;
	}

	ID3D11Texture2D* BackBuffer;
	hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(void**)&BackBuffer);

	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr), "BackBuffer", MB_OK);
		return 0;
	}

	hr = d3d11Device->CreateRenderTargetView(BackBuffer, NULL,
		&renderTargetView);
	BackBuffer->Release();

	if (FAILED(hr))
	{
		MessageBox(NULL, DXGetErrorDescription(hr), "D3D11Device", MB_OK);
		return 0;
	}

	d3d11DevCon->OMSetRenderTargets(1, &renderTargetView, NULL);
}

void ReleaseObjects()
{
	SwapChain->Release();
	d3d11Device->Release();
	d3d11DevCon->Release();
}

bool InitScene()
{
	return true;
}

void UpdateScene()
{
	red += colormodr * 0.00005f;
	green += colormodg * 0.00005f;
	blue += colormodb * 0.00005f;

	if (red >= 1.0f || red <= 0.0f)
		colormodr *= -1;
	if (green >= 1.0f || green <= 0.0f)
		colormodg *= -1;
	if (blue >= 1.0f || blue <= 0.0f)
		colormodb *= -1;
}

void DrawScene()
{
	D3DXCOLOR bgColor(red, green, blue, 1.0f);

	d3d11DevCon->ClearRenderTargetView(renderTargetView, bgColor);

	SwapChain->Present(0, 0);
}

int messageloop()
{
	MSG msg;

	ZeroMemory(&msg, sizeof(MSG));

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			UpdateScene();
			DrawScene();
		}
	}
	
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
	WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
			{
				if (MessageBox(0, "Exit?",
					"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				{
					DestroyWindow(hwnd);
				}

				return 0;
			}

		case WM_DESTROY:
			PostQuitMessage(0);

		return 0;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}