#include "stdafx.h"
#include "DXSample.h"

using namespace Microsoft::WRL;

// 构造函数，初始化 DXSample 类的实例
DXSample::DXSample(UINT width, UINT height, std::wstring name) :
    m_width(width), // 初始化宽度
    m_height(height), // 初始化高度
    m_title(name), // 初始化标题
    m_useWarpDevice(false) // 初始化是否使用 WARP 设备
{
    WCHAR assetsPath[512];
    // 获取资源路径
    GetAssetsPath(assetsPath, _countof(assetsPath));
    m_assetsPath = assetsPath;

    // 计算宽高比
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

// 析构函数，销毁 DXSample 类的实例
DXSample::~DXSample()
{
}

// 辅助函数，用于解析资源的完整路径
std::wstring DXSample::GetAssetFullPath(LPCWSTR assetName)
{
    return m_assetsPath + assetName;
}

// 辅助函数，用于获取支持 Direct3D 12 的第一个可用硬件适配器。
// 如果找不到这样的适配器，*ppAdapter 将被设置为 nullptr。
_Use_decl_annotations_
void DXSample::GetHardwareAdapter(
    IDXGIFactory1* pFactory, // DXGI 工厂接口
    IDXGIAdapter1** ppAdapter, // 输出适配器接口
    bool requestHighPerformanceAdapter) // 是否请求高性能适配器
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    // 尝试获取 IDXGIFactory6 接口
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (
            UINT adapterIndex = 0;
            SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                adapterIndex,
                requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                IID_PPV_ARGS(&adapter)));
                ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // 跳过软件适配器
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            // 检查适配器是否支持 Direct3D 12，但不创建实际设备
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    // 如果未找到适配器，尝试使用 EnumAdapters1 方法
    if (adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // 跳过软件适配器
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            // 检查适配器是否支持 Direct3D 12，但不创建实际设备
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

// 辅助函数，用于设置窗口的标题文本
void DXSample::SetCustomWindowText(LPCWSTR text)
{
    std::wstring windowText = m_title + L": " + text;
    SetWindowText(Win32Application::GetHwnd(), windowText.c_str());
}

// 辅助函数，用于解析提供的命令行参数
_Use_decl_annotations_
void DXSample::ParseCommandLineArgs(WCHAR* argv[], int argc)
{
    for (int i = 1; i < argc; ++i)
    {
        if (_wcsnicmp(argv[i], L"-warp", wcslen(argv[i])) == 0 ||
            _wcsnicmp(argv[i], L"/warp", wcslen(argv[i])) == 0)
        {
            m_useWarpDevice = true;
            m_title = m_title + L" (WARP)";
        }
    }
}
