#include "DXSample.h"

// ��ע�⣬��Ȼ ComPtr ���ڹ��� CPU ����Դ���������ڣ�
// �������˽� GPU ����Դ���������ڡ�Ӧ�ó�����뿼��
// ��Դ�� GPU �������ڣ��Ա������ٿ������� GPU ���õĶ���
// �ⷽ���һ��ʾ���������෽����OnDestroy() ���ҵ���
using Microsoft::WRL::ComPtr;

class D3D12HelloWindow : public DXSample
{
public:
    D3D12HelloWindow(UINT width, UINT height, std::wstring name);

    virtual void OnInit();    // ��ʼ��
    virtual void OnUpdate();  // ����
    virtual void OnRender();  // ��Ⱦ
    virtual void OnDestroy(); // ����

private:
    static const UINT FrameCount = 2; // ֡��

    // ���߶���
    ComPtr<IDXGISwapChain3> m_swapChain;                // ������
    ComPtr<ID3D12Device> m_device;                      // �豸
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount]; // ��ȾĿ��
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;  // ���������
    ComPtr<ID3D12CommandQueue> m_commandQueue;          // �������
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;             // ��������
    ComPtr<ID3D12PipelineState> m_pipelineState;        // ����״̬
    ComPtr<ID3D12GraphicsCommandList> m_commandList;    // �����б�
    UINT m_rtvDescriptorSize;                           // ��������С

    // ͬ������
    UINT m_frameIndex;          // ֡����
    HANDLE m_fenceEvent;        // դ���¼�
    ComPtr<ID3D12Fence> m_fence; // դ��
    UINT64 m_fenceValue;        // դ��ֵ

    void LoadPipeline();          // ���ع���
    void LoadAssets();            // ������Դ
    void PopulateCommandList();   // ��������б�
    void WaitForPreviousFrame();  // �ȴ���һ֡
};