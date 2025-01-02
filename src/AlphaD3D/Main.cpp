#include "stdafx.h"
#include "D3D12HelloWindow.h"

// ʹ��ע��� WinMain ������������ڵ�
_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // ���� ClearType
    BOOL fClearType = TRUE;
    SystemParametersInfo(SPI_SETFONTSMOOTHING, TRUE, &fClearType, 0);
    SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE, 0, (PVOID)FE_FONTSMOOTHINGCLEARTYPE, 0);

    try
    {
        // ���� D3D12HelloWindow ʵ������ʼ�����ڴ�С�ͱ���
        D3D12HelloWindow sample(1280, 720, L"D3D12 Hello Window");
        // ����Ӧ�ó���
        return Win32Application::Run(&sample, hInstance, nCmdShow);
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "��׽��ȫ���쳣", MB_OK | MB_ICONERROR);
        return -1;
    }
    catch (...)
    {
        MessageBoxA(nullptr, "����δ֪����", "��׽��ȫ���쳣", MB_OK | MB_ICONERROR);
        return -1;
    }
}
