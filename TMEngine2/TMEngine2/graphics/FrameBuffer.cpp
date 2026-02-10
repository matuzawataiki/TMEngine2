#include "Precompiler.h"
#include "FrameBuffer.h"
namespace nsTMEngine2
{
	bool FrameBuffer::CreateRTV(ID3D12Device* d3dDevice)
	{

		DXGI_SWAP_CHAIN_DESC swcDesc = {};
		auto result = m_swapChain->GetDesc(&swcDesc);
		if (FAILED(result)) {
			MessageBox(nullptr, L"スワップチェインの情報の取得に失敗", L"エラー", MB_OK);
		}

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

		for (int i = 0; i < swcDesc.BufferCount; i++) {
			result = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
			if (FAILED(result)) {
				MessageBox(nullptr, L"スワップチェインのバッファの取得に失敗", L"エラー", MB_OK);
			}
			rtvHandle.ptr += i * d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			d3dDevice->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
		}


	}
}
