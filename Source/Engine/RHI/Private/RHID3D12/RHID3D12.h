#pragma once
#include "RHI\Public\RHI.h"
#include <d3d12.h>
#include <dxgi1_4.h>
namespace Engine {
	class RHID3D12 final: RHI {
	private:
		static const uint8 BACK_BUFFER_COUNT = 2;
		RFormat                 m_SwapchainFormat{ FORMAT_B8G8R8A8_UNORM };
		RFormat                 m_DepthFormat{ FORMAT_D24_UNORM_S8_UINT };
		uint32                  m_MsaaSampleCount{ 4 };
		uint32                  m_MsaaQuality{ 0 };
		bool                    m_EnableMsaa{ false };

		IDXGIFactory4* m_DXGIFactory;
		ID3D12Device* m_Device{ nullptr };
		ID3D12Fence* m_Fence{ nullptr };
		uint32 m_CurrentFence{ 0 };

		ID3D12CommandQueue* m_CommandQueue{ nullptr };
		ID3D12CommandAllocator* m_CommandAllocator{ nullptr };
		ID3D12GraphicsCommandList* m_GraphicsCommandList{ nullptr };

		IDXGISwapChain* m_Swapchain{ nullptr };
		ID3D12Resource* m_SwapchainBuffer[BACK_BUFFER_COUNT];
		uint32 m_CurrentBackBuffer{ 0 };
		ID3D12Resource* m_DepthStencilBuffer{ nullptr };
		uint32 m_RtvDescriptorSize = 0;//Render Target View
		ID3D12DescriptorHeap* m_RTVDescriptorHeap;
		ID3D12DescriptorHeap* m_DSVDescriptorHeap;

		USize2D m_SwapchainExtent{};
		D3D12_VIEWPORT m_Viewport;
		D3D12_RECT m_ScissorRect;

	private:
		void CreateDeviceContext();
		void CreateCommandObjects();
		void CreateSwapchain(HWND wnd);
		void CreateInternalDescriptorHeaps();
		void FlushCommandQueue();
		void OnResized();

		typedef std::function<void(ID3D12GraphicsCommandList* cmdList)> CmdFunc;
		void InternalImmediateCommit(CmdFunc&& func);
	public:
		RHID3D12() = default;
		~RHID3D12() = default;
		void Initialize(const RSInitInfo* initInfo) override;
		void Release() override;

		ID3D12Device* Device() { return m_Device; }
		const USize2D& GetSwapchainExtent() override { return m_SwapchainExtent; }
		RFormat GetSwapchainImageFormat() override { return m_SwapchainFormat; }
		RFormat GetDepthFormat() override { return m_DepthFormat; }
		RImageView* GetSwapchainImageView(uint8 i) override;
		uint32 GetSwapchainMaxImageCount() override { return 1; }
		void ResizeSwapchain(uint32 width, uint32 height) override;
		static RHID3D12* InstanceD3D12() { return dynamic_cast<RHID3D12*>(RHI::Instance()); }
	};

#define DX_DEVICE  RHID3D12::InstanceD3D12()->Device()
#define DX_CONTEXT RHID3D12::InstanceD3D12()->Context()
}