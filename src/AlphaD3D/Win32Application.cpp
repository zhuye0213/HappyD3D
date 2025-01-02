#include "stdafx.h"
#include "Win32Application.h"
#include <iostream>
#include "DebugUtils.h" 


// 静态成员变量，存储窗口句柄
HWND Win32Application::m_hwnd = nullptr;

// 运行应用程序的主函数
int Win32Application::Run(DXSample* pSample, HINSTANCE hInstance, int nCmdShow)
{
#ifdef _DEBUG
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);   
#endif

	DEBUG_LOG("程序启动...");

    // 解析命令行参数
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    pSample->ParseCommandLineArgs(argv, argc);
    LocalFree(argv);

    // 初始化窗口类
    WNDCLASSEX windowClass = { 0 };
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DXSampleClass";
    RegisterClassEx(&windowClass);

    // 设置窗口大小
    RECT windowRect = { 0, 0, static_cast<LONG>(pSample->GetWidth()), static_cast<LONG>(pSample->GetHeight()) };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // 创建窗口并存储句柄
    m_hwnd = CreateWindow(
        windowClass.lpszClassName,
        pSample->GetTitle(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,        // 无父窗口
        nullptr,        // 无菜单
        hInstance,
        pSample);

    // 初始化示例，OnInit 在 DXSample 的每个子类中定义
    pSample->OnInit();

    // 显示窗口
    ShowWindow(m_hwnd, nCmdShow);

    // 主消息循环
    MSG msg = {};
    while (msg.message != WM_QUIT)
    {
        // 处理队列中的消息
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // 销毁示例
    pSample->OnDestroy();

    // 返回 WM_QUIT 消息的 wParam 部分
    return static_cast<char>(msg.wParam);
}

// 窗口的主消息处理函数
LRESULT CALLBACK Win32Application::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // 获取与窗口关联的 DXSample 实例
    DXSample* pSample = reinterpret_cast<DXSample*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
    {
        // 保存传递给 CreateWindow 的 DXSample 指针
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

    // 处理未在 switch 语句中处理的消息
    return DefWindowProc(hWnd, message, wParam, lParam);
}

