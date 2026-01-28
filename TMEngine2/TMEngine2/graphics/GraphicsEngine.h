
#pragma once
namespace nsTMEngine2
{
	struct Vertex{ 
		XMFLOAT3 position; 
		XMFLOAT4 color; 
	};

	class GraphicsEngine : public Noncopyable
	{
		//シングルトン
	private:
		static GraphicsEngine* instans;
		GraphicsEngine();

		//非公開変数
	private:
		WNDCLASSEX m_wndclassEx = {};	//ウィンドウクラス
		HWND m_hwnd = {};				//ウィンドウハンドル

		MSG m_msg = {};

		ID3D12Device* m_dev = nullptr;
		IDXGIFactory6* m_dxgiFactory = nullptr;
		ID3D12CommandAllocator* m_commandAllocator = nullptr;
		ID3D12GraphicsCommandList* m_commandList = nullptr;
		ID3D12CommandQueue* m_commandQueue = nullptr;
		IDXGISwapChain4* m_swapchain = nullptr;
		ID3D12DescriptorHeap* m_rtvheaps = nullptr;
		std::vector<ID3D12Resource*> m_backBuffers;
		ID3D12Fence* _fence = nullptr;


		//非公開関数
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

		static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);


		//公開関数
	public:
		~GraphicsEngine();

		void Initialize();

		void Execute();

		void CreateTriangle();



		static void CreateInstans() {
			if (instans == nullptr) {
				instans = new GraphicsEngine;
			}
		}

		static void DeleteInstans() {
			if (instans != nullptr) {
				delete instans;
				instans = nullptr;
			}
		}

		static GraphicsEngine* GetInstans() {
			return instans;
		}
	};

}
