#include "Precompiler.h"
#include "GraphicsEngine.h"

namespace {
	const uint8_t window_width = 1280;	//ウィンドウ幅
	const uint8_t window_height = 720;	//ウィンドウ高さ
}

GraphicsEngine* GraphicsEngine::instans = nullptr;

GraphicsEngine::GraphicsEngine()
{
}

GraphicsEngine::~GraphicsEngine()
{
}




void GraphicsEngine::Initialize()
{
	InitializeWindow();

}



void GraphicsEngine::InitializeWindow()
{
	m_wndclassEx.cbClsExtra = sizeof(WNDCLASSEX);			//サイズ設定
	m_wndclassEx.lpfnWndProc = (WNDPROC)WindowProcedure;	//プロシージャの設定
	m_wndclassEx.lpszClassName = _T("TMEngine2");			//クラスの名前の設定
	m_wndclassEx.hInstance = GetModuleHandle(nullptr);		//アプリのインスタンスハンドル

	RegisterClassEx(&m_wndclassEx);		//ウィンドウクラスの設定

	RECT wrc = { 0, 0, window_width, window_height };		//ウィンドウのサイズを設定
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);		//ウィンドウのサイズを調整

	//ウィンドウの生成	
	m_hwnd = CreateWindow(
		m_wndclassEx.lpszClassName,
		_T("三角形の表示"),		//ウィンドウの名前
		WS_OVERLAPPEDWINDOW,	//ウィンドウのスタイル
		CW_USEDEFAULT,			//X座標
		CW_USEDEFAULT,			//y座標
		wrc.right - wrc.left,	//幅
		wrc.bottom - wrc.top,	//高さ
		nullptr,				//親ウィンドウ			
		nullptr,				//メニュー
		m_wndclassEx.hInstance,	//アプリのインスタンスハンドル
		nullptr,				//追加パラメータ
		);

	ShowWindow(m_hwnd, SW_SHOW);	//ウィンドウを表示	
}

void GraphicsEngine::CreateFactory()
{
#ifdef _DEBUG
	// デバッグ時はデバッグ機能を有効にしたファクトリーを作る
	auto result = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory));
#else 
	// リリース時は通常のファクトリーを作る
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));
#endif

	if (FAILED(result)) {
		MessageBox(nullptr, L"ファクトリーの作成に失敗", L"エラー", MB_OK);
	}
}

void GraphicsEngine::CreateAdapter()
{
	std::vector<IDXGIAdapter*> adapters;	//アダプタのリスト
	IDXGIAdapter* tmpAdapter = nullptr;		//使用するアダプタ

	//すべてのアダプタをリストアップ
	for (int i = 0; m_dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++) {
		adapters.push_back(tmpAdapter);
	}

	//RTXシリーズのGPUを探す
	for (auto adapter : adapters) {
		DXGI_ADAPTER_DESC adapterDesc = {};
		adapter->GetDesc(&adapterDesc);
		std::wstring strDesc = adapterDesc.Description;

		if (strDesc.find(L"RTX") != std::string::npos) {
			tmpAdapter = adapter;
			break;
		}
	}

	//デバイスの作成
	auto result = D3D12CreateDevice(
		tmpAdapter,
		D3D_FEATURE_LEVEL_12_1,
		IID_PPV_ARGS(&m_dev)
	);

	if (FAILED(result)) {
		MessageBox(nullptr, L"デバイスの作成に失敗", L"エラー", MB_OK);
	}

}

void GraphicsEngine::CreateCommands()
{
	//コマンドアロケーターの作成
	auto result = m_dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, 
		IID_PPV_ARGS(&m_commandAllocator)
	);

	if (FAILED(result)) {
		MessageBox(nullptr, L"コマンドアロケータの作成に失敗", L"エラー", MB_OK);
	}

	//コマンドリストの作成
	result = m_dev->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_commandAllocator,
		nullptr,
		IID_PPV_ARGS(&m_commandList)
	);

	if (FAILED(result)) {
		MessageBox(nullptr, L"コマンドリストの作成に失敗", L"エラー", MB_OK);
	}

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};

	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	result = m_dev->CreateCommandQueue(
		&commandQueueDesc,
		IID_PPV_ARGS(&m_commandQueue)
	);

	if (FAILED(result)) {
		MessageBox(nullptr, L"コマンドキューの作成に失敗", L"エラー", MB_OK);
	}
}

void GraphicsEngine::CreateSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

	swapChainDesc.Width = window_width;
	swapChainDesc.Height = window_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto result = m_dxgiFactory->CreateSwapChainForHwnd(
		m_commandQueue,
		m_hwnd,
		&swapChainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)&m_swapchain
	);

	if (FAILED(result)) {
		MessageBox(nullptr, L"スワップチェインの作成に失敗", L"エラー", MB_OK);
	}
}

void GraphicsEngine::CreateDescriptorHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	heapDesc.NumDescriptors = 2;

	auto result = m_dev->CreateDescriptorHeap(
		&heapDesc,
		IID_PPV_ARGS(&m_rtvheaps)
	);
	if (FAILED(result)) {
		MessageBox(nullptr, L"ディスクリプタヒープの作成に失敗", L"エラー", MB_OK);
	}
}

void GraphicsEngine::CreateRenderTargetView()
{
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	auto result = m_swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		MessageBox(nullptr, L"スワップチェインの情報の取得に失敗", L"エラー", MB_OK);
	}

	m_backBuffers.resize(swcDesc.BufferCount);

	for (int i = 0; i < swcDesc.BufferCount; i++) {
		result = m_swapchain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i]));
		if (FAILED(result)) {
			MessageBox(nullptr, L"スワップチェインのバッファの取得に失敗", L"エラー", MB_OK);
		}
		D3D12_CPU_DESCRIPTOR_HANDLE handle = m_rtvheaps->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += i * m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		m_dev->CreateRenderTargetView(m_backBuffers[i], nullptr, handle);
	}


}



LRESULT CALLBACK GraphicsEngine::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);

}
