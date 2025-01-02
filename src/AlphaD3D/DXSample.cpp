#include "stdafx.h"
#include "DXSample.h"

using namespace Microsoft::WRL;

// ���캯������ʼ�� DXSample ���ʵ��
DXSample::DXSample(UINT width, UINT height, std::wstring name) :
    m_width(width), // ��ʼ�����
    m_height(height), // ��ʼ���߶�
    m_title(name), // ��ʼ������
    m_useWarpDevice(false) // ��ʼ���Ƿ�ʹ�� WARP �豸
{
    WCHAR assetsPath[512];
    // ��ȡ��Դ·��
    GetAssetsPath(assetsPath, _countof(assetsPath));
    m_assetsPath = assetsPath;

    // �����߱�
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

// �������������� DXSample ���ʵ��
DXSample::~DXSample()
{
}

// �������������ڽ�����Դ������·��
std::wstring DXSample::GetAssetFullPath(LPCWSTR assetName)
{
    return m_assetsPath + assetName;
}

// �������������ڻ�ȡ֧�� Direct3D 12 �ĵ�һ������Ӳ����������
// ����Ҳ�����������������*ppAdapter ��������Ϊ nullptr��
_Use_decl_annotations_
void DXSample::GetHardwareAdapter(
    IDXGIFactory1* pFactory, // DXGI �����ӿ�
    IDXGIAdapter1** ppAdapter, // ����������ӿ�
    bool requestHighPerformanceAdapter) // �Ƿ����������������
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    // ���Ի�ȡ IDXGIFactory6 �ӿ�
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

            // �������������
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            // ����������Ƿ�֧�� Direct3D 12����������ʵ���豸
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    // ���δ�ҵ�������������ʹ�� EnumAdapters1 ����
    if (adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // �������������
            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                continue;
            }

            // ����������Ƿ�֧�� Direct3D 12����������ʵ���豸
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

// �����������������ô��ڵı����ı�
void DXSample::SetCustomWindowText(LPCWSTR text)
{
    std::wstring windowText = m_title + L": " + text;
    SetWindowText(Win32Application::GetHwnd(), windowText.c_str());
}

// �������������ڽ����ṩ�������в���
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
