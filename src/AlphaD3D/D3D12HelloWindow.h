#include "DXSample.h"

// 请注意，虽然 ComPtr 用于管理 CPU 上资源的生命周期，
// 但它不了解 GPU 上资源的生命周期。应用程序必须考虑
// 资源的 GPU 生命周期，以避免销毁可能仍由 GPU 引用的对象。
// 这方面的一个示例可以在类方法：OnDestroy() 中找到。
using Microsoft::WRL::ComPtr;

class D3D12HelloWindow : public DXSample
{
public:
    D3D12HelloWindow(UINT width, UINT height, std::wstring name);

    virtual void OnInit();    // 初始化
    virtual void OnUpdate();  // 更新
    virtual void OnRender();  // 渲染
    virtual void OnDestroy(); // 销毁

private:
    static const UINT FrameCount = 2; // 帧数

    // 管线对象。
    ComPtr<IDXGISwapChain3> m_swapChain;                // 交换链
    ComPtr<ID3D12Device> m_device;                      // 设备
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount]; // 渲染目标
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;  // 命令分配器
    ComPtr<ID3D12CommandQueue> m_commandQueue;          // 命令队列
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;             // 描述符堆
    ComPtr<ID3D12PipelineState> m_pipelineState;        // 管线状态
    ComPtr<ID3D12GraphicsCommandList> m_commandList;    // 命令列表
    UINT m_rtvDescriptorSize;                           // 描述符大小

    // 同步对象。
    UINT m_frameIndex;          // 帧索引
    HANDLE m_fenceEvent;        // 栅栏事件
    ComPtr<ID3D12Fence> m_fence; // 栅栏
    UINT64 m_fenceValue;        // 栅栏值

    void LoadPipeline();          // 加载管线
    void LoadAssets();            // 加载资源
    void PopulateCommandList();   // 填充命令列表
    void WaitForPreviousFrame();  // 等待上一帧
};