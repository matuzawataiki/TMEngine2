#pragma once


namespace nsTMEngine2
{
	using Microsoft::WRL::ComPtr;

	struct Vertex{ 
		XMFLOAT3 position; 
		XMFLOAT4 color; 
	};
	
	class GraphicsEngine : public Noncopyable
	{
		//シングルトン
	private:
		static GraphicsEngine* instance;
		GraphicsEngine();

	private:
		WNDCLASSEX m_wndclassEx = {};	//ウィンドウクラス
		HWND m_hwnd = {};				//ウィンドウハンドル

		MSG m_msg = {};

		ComPtr<ID3D12Device> m_dev;
		ComPtr<IDXGIFactory6> m_dxgiFactory;
		ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		ComPtr<ID3D12GraphicsCommandList> m_commandList;
		ComPtr<ID3D12CommandQueue> m_commandQueue;
		ComPtr<IDXGISwapChain4> m_swapchain;
		ComPtr<ID3D12DescriptorHeap> m_rtvheaps;
		std::vector<ComPtr<ID3D12Resource>> m_backBuffers;
		ComPtr<ID3D12Fence> _fence;
		ComPtr<ID3D12RootSignature> m_rootsignature;
		ComPtr<ID3D12PipelineState> m_pipelineState;
		ComPtr<ID3DBlob> m_rootSigBlob;

	private:
		/**
		 * @brief ウィンドウの生成初期化
		 */
		void InitializeWindow();

		void CreateFactory();

		void CreateAdapter();

		void CreateCommands();

		void CreateSwapChain();

		void CreateDescriptorHeap();

		void CreateRenderTargetView();

		void SetGraphicsPipeline();

		static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


	public:
		~GraphicsEngine();

		void Initialize();

		void Execute();

		void CreateTriangle();

		void Draw(Vertex& vertex);

		static void CreateInstance() {
			if (instance == nullptr) {
				instance = new GraphicsEngine;
			}
		}

		static void DeleteInstance() {
			if (instance != nullptr) {
				delete instance;
				instance = nullptr;
			}
		}

		static GraphicsEngine* GetInstance() {
			return instance;
		}
	};

}
