#pragma once

#include "DXSampleHelper.h"
#include "Win32Application.h"

// DXSample �ඨ��
class DXSample
{
public:
    // ���캯������ʼ����ȡ��߶Ⱥ�����
    DXSample(UINT width, UINT height, std::wstring name);
    // ����������
    virtual ~DXSample();

    // ���麯�����������ʵ����Щ����
    virtual void OnInit() = 0;    // ��ʼ��
    virtual void OnUpdate() = 0;  // ����
    virtual void OnRender() = 0;  // ��Ⱦ
    virtual void OnDestroy() = 0; // ����

    // ���������д��Щ�¼��������������ض�����Ϣ
    virtual void OnKeyDown(UINT8 /*key*/) {} // ���������¼�
    virtual void OnKeyUp(UINT8 /*key*/) {} // �����ͷ��¼�

    // ����������
    UINT GetWidth() const { return m_width; }  // ��ȡ���
    UINT GetHeight() const { return m_height; } // ��ȡ�߶�
    const WCHAR* GetTitle() const { return m_title.c_str(); } // ��ȡ���ڱ���

    // ���������в���
    void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

protected:
    // ��ȡ��Դ������·��
    std::wstring GetAssetFullPath(LPCWSTR assetName);

    // ��ȡӲ��������
    void GetHardwareAdapter(
        _In_ IDXGIFactory1* pFactory,
        _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
        bool requestHighPerformanceAdapter = false);

    // �����Զ��崰���ı�
    void SetCustomWindowText(LPCWSTR text);

    // �ӿڳߴ�
    UINT m_width;       // ���
    UINT m_height;      // �߶�
    float m_aspectRatio; // �ݺ��

    // ��������Ϣ
    bool m_useWarpDevice; // �Ƿ�ʹ��WARP�豸

private:
    // ����Դ·��
    std::wstring m_assetsPath;

    // ���ڱ���
    std::wstring m_title;
};
