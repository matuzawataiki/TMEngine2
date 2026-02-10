#pragma once
namespace nsTMEngine2
{
	using Microsoft::WRL::ComPtr;

	class FrameBuffer
	{
	private:
		enum { FRAME_BUFFER_COUNT = 2 };
		ComPtr<ID3D12DescriptorHeap> m_rtvHeap = nullptr;
		ComPtr<ID3D12DescriptorHeap> m_dsvhHeap = nullptr;
		UINT m_rtvDescriptorSize = 0;
		UINT m_dsvDescriptorSize = 0;
		ComPtr<ID3D12Resource> m_renderTargets[FRAME_BUFFER_COUNT] = { nullptr };
		ComPtr<ID3D12Resource> m_depthStencilBuffer = nullptr;
		D3D12_RECT m_scissorRect;
		D3D12_VIEWPORT m_viewport;
		D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferRTVHandle;
		D3D12_CPU_DESCRIPTOR_HANDLE m_currentFrameBufferDSVHandle;
		UINT m_backBufferIndex = 0;
		IDXGISwapChain4* m_swapChain = nullptr;

	private:
		bool CreateRTV(ID3D12Device* d3dDevice);
	};
}

