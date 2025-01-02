#include "stdafx.h"
#include "D3D12HelloWindow.h"

// D3D12HelloWindow ���캯������ʼ�����ڿ�ȡ��߶Ⱥ�����
D3D12HelloWindow::D3D12HelloWindow(UINT width, UINT height, std::wstring name) :
	DXSample(width, height, name),
	m_frameIndex(0),
	m_rtvDescriptorSize(0)
{
}

// ��ʼ��������������Ⱦ���ߺ���Դ
void D3D12HelloWindow::OnInit()
{
	LoadPipeline();
	LoadAssets();
}

// ������Ⱦ����������
void D3D12HelloWindow::LoadPipeline()
{
	UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
	// ���õ��Բ㣨��Ҫͼ�ι��ߡ���ѡ���ܡ�����
	// ע�⣺���豸���������õ��Բ㽫ʹ��豸��Ч��
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();

			// ���ö���ĵ��Բ㡣
			dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		}
	}
#endif

	ComPtr<IDXGIFactory4> factory;
	ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

	if (m_useWarpDevice)
	{
		ComPtr<IDXGIAdapter> warpAdapter;
		ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			warpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		));
	}
	else
	{
		ComPtr<IDXGIAdapter1> hardwareAdapter;
		GetHardwareAdapter(factory.Get(), &hardwareAdapter);

		ThrowIfFailed(D3D12CreateDevice(
			hardwareAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_device)
		));
	}

	// ����������������С�
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

	// ������������������
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.BufferCount = FrameCount;
	swapChainDesc.Width = m_width;
	swapChainDesc.Height = m_height;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc.Count = 1;

	ComPtr<IDXGISwapChain1> swapChain;
	ThrowIfFailed(factory->CreateSwapChainForHwnd(
		m_commandQueue.Get(),        // ��������Ҫ���У��Ա�������ǿ��ˢ�¡�
		Win32Application::GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain
	));

	// ��ʾ����֧��ȫ���л���
	ThrowIfFailed(factory->MakeWindowAssociation(Win32Application::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

	ThrowIfFailed(swapChain.As(&m_swapChain));
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	// �����������ѡ�
	{
		// ����������һ����ȾĿ����ͼ��RTV���������ѡ�
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = FrameCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// ����֡��Դ��
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Ϊÿһ֡����һ�� RTV��
		for (UINT n = 0; n < FrameCount; n++)
		{
			ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
			m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, m_rtvDescriptorSize);
		}
	}

	ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));
}

// ����ʾ����Դ��
void D3D12HelloWindow::LoadAssets()
{
	// ���������б�
	ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)));

	// �����б��ڴ���ʱ���ڼ�¼״̬����Ŀǰû��Ҫ��¼�����ݡ�
	// ��ѭ���������ǹرյģ��������ڹر�����
	ThrowIfFailed(m_commandList->Close());

	// ����ͬ������
	{
		ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
		m_fenceValue = 1;

		// ����һ���¼����������֡ͬ����
		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_fenceEvent == nullptr)
		{
			ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
		}
	}
}

// ���»���֡��ֵ��
void D3D12HelloWindow::OnUpdate()
{
}

// ��Ⱦ������
void D3D12HelloWindow::OnRender()
{
	// ��¼������Ҫ��Ⱦ��������������б��С�
	PopulateCommandList();

	// ִ�������б�
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// ����֡��
	ThrowIfFailed(m_swapChain->Present(1, 0));

	WaitForPreviousFrame();
}

// ���ٺ�����������Դ��
void D3D12HelloWindow::OnDestroy()
{
	// ȷ�� GPU �������ü����������������Դ��
	WaitForPreviousFrame();

	CloseHandle(m_fenceEvent);
}

// ��������б�
void D3D12HelloWindow::PopulateCommandList()
{
	// ֻ���ڹ����������б��� GPU ��ִ�����ʱ���������������б��������
	// Ӧ�ó���Ӧʹ��դ����ȷ�� GPU ִ�н��ȡ�
	ThrowIfFailed(m_commandAllocator->Reset());

	// ���ǣ������ض������б��ϵ��� ExecuteCommandList() ʱ��
	// �������б������ʱ���ã����ұ��������¼�¼֮ǰ���á�
	ThrowIfFailed(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()));

	// ָʾ��̨��������������ȾĿ�ꡣ
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	m_commandList->ResourceBarrier(1, &barrier);
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);

	// ��¼���
	const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// ָʾ��̨���������ڽ����ڳ��֡�
	CD3DX12_RESOURCE_BARRIER barrier1 = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	m_commandList->ResourceBarrier(1, &barrier1);

	ThrowIfFailed(m_commandList->Close());
}

// �ȴ���һ֡��ɡ�
void D3D12HelloWindow::WaitForPreviousFrame()
{
	// �ȴ�֡��ɺ��ټ����������ʵ����
	// ����Ϊ�˼򻯶�ʵ�ֵĴ��롣D3D12HelloFrameBuffering
	// ʾ��չʾ�����ʹ��դ������Ч��ʹ����Դ����� GPU �����ʡ�

	// �����źŲ�����դ��ֵ��
	const UINT64 fence = m_fenceValue;
	ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fence));
	m_fenceValue++;

	// �ȴ���һ֡��ɡ�
	if (m_fence->GetCompletedValue() < fence)
	{
		ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

