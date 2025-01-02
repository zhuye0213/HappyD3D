#pragma once

#include "DXSample.h"

// 前向声明 DXSample 类
class DXSample;

// Win32Application 类定义
class Win32Application
{
public:
    // 运行应用程序
    static int Run(DXSample* pSample, HINSTANCE hInstance, int nCmdShow);
    // 获取窗口句柄
    static HWND GetHwnd() { return m_hwnd; }

protected:
    // 窗口过程回调函数
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    // 窗口句柄
    static HWND m_hwnd;
};

