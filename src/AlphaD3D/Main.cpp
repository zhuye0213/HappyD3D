#include "stdafx.h"
#include "D3D12HelloWindow.h"

// 使用注解的 WinMain 函数，程序入口点
_Use_decl_annotations_
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
    // 启用 ClearType
    BOOL fClearType = TRUE;
    SystemParametersInfo(SPI_SETFONTSMOOTHING, TRUE, &fClearType, 0);
    SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE, 0, (PVOID)FE_FONTSMOOTHINGCLEARTYPE, 0);

    try
    {
        // 创建 D3D12HelloWindow 实例，初始化窗口大小和标题
        D3D12HelloWindow sample(1280, 720, L"D3D12 Hello Window");
        // 运行应用程序
        return Win32Application::Run(&sample, hInstance, nCmdShow);
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "捕捉到全局异常", MB_OK | MB_ICONERROR);
        return -1;
    }
    catch (...)
    {
        MessageBoxA(nullptr, "发生未知错误。", "捕捉到全局异常", MB_OK | MB_ICONERROR);
        return -1;
    }
}
