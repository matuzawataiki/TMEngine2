#include "Precompiler.h"
#include "GraphicsEngine.h"


namespace nsTMEngine2
{
	namespace {
		const uint8_t window_width = 1280;	//ウィンドウ幅
		const uint8_t window_height = 720;	//ウィンドウ高さ
	}

	GraphicsEngine* GraphicsEngine::instance = nullptr;

	GraphicsEngine::GraphicsEngine()
	{
	}

	GraphicsEngine::~GraphicsEngine()
	{
	}



	void GraphicsEngine::Initialize()
	{
		InitializeWindow();

		CreateFactory();

		CreateAdapter();

		CreateCommands();

		CreateSwapChain();

		CreateDescriptorHeap();

		CreateRenderTargetView();

		SetGraphicsPipeline();
	}

	void GraphicsEngine::Execute()
	{
		auto result = m_commandAllocator->Reset();

		auto bbIdx = m_swapchain->GetCurrentBackBufferIndex();

		auto rtvH = m_rtvheaps->GetCPUDescriptorHandleForHeapStart();
		rtvH.ptr += bbIdx *m_dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		
		m_commandList->OMSetRenderTargets(1, &rtvH, true, nullptr);

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_backBuffers[bbIdx].Get();
		barrier.Transition.Subresource = 0;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;

		m_commandList->ResourceBarrier(1, &barrier);


		if (FAILED(result)) {
			MessageBox(nullptr, L"コマンドアロケーターのリセットに失敗", L"エラー", MB_OK);
		}

	}

	void GraphicsEngine::CreateTriangle()
	{
		Vertex vertices[] = {
			{{-0.5f,-0.7f,0.0f},	{1.0f,0.0f,0.0f,1.0f},},
			{{ 0.0f, 0.7f,0.0f},	{0.0f,1.0f,0.0f,1.0f},},
			{{ 0.5f,-0.7f,0.0f},	{0.0f,0.0f,1.0f,1.0f},},
		};

		D3D12_HEAP_PROPERTIES heapprop = {};

		heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC resdesc = {};

		resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		resdesc.Width = sizeof(vertices);
		resdesc.Height = 1;
		resdesc.DepthOrArraySize = 1;
		resdesc.MipLevels = 1;
		resdesc.Format = DXGI_FORMAT_UNKNOWN;
		resdesc.SampleDesc.Count = 1;
		resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
		resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		ID3D12Resource* vertBuff = nullptr;

		auto result = m_dev->CreateCommittedResource(
			&heapprop,
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&vertBuff)
		);

		Vertex* vertMap = nullptr;
		result = vertBuff->Map(0, nullptr, (void**)&vertMap);
		copy(begin(vertices), end(vertices), vertMap);
		vertBuff->Unmap(0, nullptr);

		D3D12_VERTEX_BUFFER_VIEW vbView = {};

		vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
		vbView.SizeInBytes = sizeof(vertices);
		vbView.StrideInBytes = sizeof(vertices[0]);
	}

	void GraphicsEngine::Draw(Vertex& vertex)
	{
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
			m_commandAllocator.Get(),
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

		ComPtr<IDXGISwapChain1> swapChain1;

		auto result = m_dxgiFactory->CreateSwapChainForHwnd(
			m_commandQueue.Get(),
			m_hwnd,
			&swapChainDesc,
			nullptr,
			nullptr,
			&swapChain1 
		);

		if (SUCCEEDED(result)) {
			result = swapChain1.As(&m_swapchain);
		}
		else if (FAILED(result)) {
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
			m_dev->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, handle);
		}
	}

	void GraphicsEngine::SetGraphicsPipeline()
	{
		ID3DBlob* vsBlob = nullptr;
		ID3DBlob* psBlob = nullptr;

		ID3DBlob* errorBlob = nullptr;

		auto result = D3DCompileFromFile(
			L"BasicVertexShader.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"VSMain",
			"vs_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&vsBlob, &errorBlob
		);

		if (FAILED(result)) {
			MessageBox(nullptr, L"頂点シェーダーの読み込みに失敗", L"エラー", MB_OK);
		}

		result = D3DCompileFromFile(
			L"BasicPixelShader.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"PSMain",
			"ps_5_0",
			D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
			0,
			&psBlob, &errorBlob
		);

		if (FAILED(result)) {
			MessageBox(nullptr, L"ピクセルシェーダーの読み込みに失敗", L"エラー", MB_OK);
		}

		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D12_APPEND_ALIGNED_ELEMENT,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
		gpipeline.pRootSignature = nullptr;
		gpipeline.VS.pShaderBytecode = vsBlob->GetBufferPointer();
		gpipeline.VS.BytecodeLength = vsBlob->GetBufferSize();
		gpipeline.PS.pShaderBytecode = psBlob->GetBufferPointer();
		gpipeline.PS.BytecodeLength = psBlob->GetBufferSize();
		gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		gpipeline.RasterizerState.MultisampleEnable = false;
		gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		gpipeline.RasterizerState.DepthClipEnable = true;
		gpipeline.BlendState.AlphaToCoverageEnable = false;
		gpipeline.BlendState.IndependentBlendEnable = false;

		D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
		renderTargetBlendDesc.BlendEnable = false;
		renderTargetBlendDesc.LogicOpEnable = false;
		renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;
		gpipeline.InputLayout.pInputElementDescs = inputLayout;
		gpipeline.InputLayout.NumElements = _countof(inputLayout);
		gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		gpipeline.NumRenderTargets = 1;
		gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		gpipeline.SampleDesc.Count = 1;
		gpipeline.SampleDesc.Quality = 0;

		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, m_rootSigBlob.GetAddressOf(), &errorBlob);
		m_dev->CreateRootSignature(0, m_rootSigBlob->GetBufferPointer(), m_rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootsignature));
		m_dev->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&m_pipelineState));

		m_rootSigBlob->Release();
		gpipeline.pRootSignature = m_rootsignature.Get();

	}



	LRESULT CALLBACK GraphicsEngine::WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (msg == WM_DESTROY) {
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd, msg, wparam, lparam);

	}
}
