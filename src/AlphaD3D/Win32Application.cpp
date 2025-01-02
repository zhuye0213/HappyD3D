#include "stdafx.h"
#include "Win32Application.h"
#include <iostream>
#include "DebugUtils.h" 


// ��̬��Ա�������洢���ھ��
HWND Win32Application::m_hwnd = nullptr;

// ����Ӧ�ó����������
int Win32Application::Run(DXSample* pSample, HINSTANCE hInstance, int nCmdShow)
{
#ifdef _DEBUG
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);   
#endif

	DEBUG_LOG("��������...");

    // ���������в���
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    pSample->ParseCommandLineArgs(argv, argc);
    LocalFree(argv);

    // ��ʼ��������
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DXSampleClass";
    RegisterClassEx(&windowClass);

    // ���ô��ڴ�С
    RECT windowRect = { 0, 0, static_cast<LONG>(pSample->GetWidth()), static_cast<LONG>(pSample->GetHeight()) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // �������ڲ��洢���
    m_hwnd = CreateWindow(
        windowClass.lpszClassName,
        pSample->GetTitle(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,        // �޸�����
        nullptr,        // �޲˵�
        hInstance,
        pSample);

    // ��ʼ��ʾ����OnInit �� DXSample ��ÿ�������ж���
    pSample->OnInit();

    // ��ʾ����
    ShowWindow(m_hwnd, nCmdShow);

    // ����Ϣѭ��
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        // ��������е���Ϣ
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // ����ʾ��
    pSample->OnDestroy();

    // ���� WM_QUIT ��Ϣ�� wParam ����
    return static_cast<char>(msg.wParam);
}

// ���ڵ�����Ϣ������
LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // ��ȡ�봰�ڹ����� DXSample ʵ��
    DXSample* pSample = reinterpret_cast<DXSample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
    {
        // ���洫�ݸ� CreateWindow �� DXSample ָ��
        LPCREATESTRUCT pCreateStruct = reinterpret_cast<LPCREATESTRUCT>(lParam);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pCreateStruct->lpCreateParams));
    }
    return 0;

    case WM_KEYDOWN:
        if (pSample)
        {
            pSample->OnKeyDown(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_KEYUP:
        if (pSample)
        {
            pSample->OnKeyUp(static_cast<UINT8>(wParam));
        }
        return 0;

    case WM_PAINT:
        if (pSample)
        {
            pSample->OnUpdate();
            pSample->OnRender();
        }
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    // ����δ�� switch ����д������Ϣ
    return DefWindowProc(hWnd, message, wParam, lParam);
}

