#pragma once

#include "DXSampleHelper.h"
#include "Win32Application.h"

// DXSample 类定义
class DXSample
{
public:
    // 构造函数，初始化宽度、高度和名称
    DXSample(UINT width, UINT height, std::wstring name);
    // 虚析构函数
    virtual ~DXSample();

    // 纯虚函数，子类必须实现这些方法
    virtual void OnInit() = 0;    // 初始化
    virtual void OnUpdate() = 0;  // 更新
    virtual void OnRender() = 0;  // 渲染
    virtual void OnDestroy() = 0; // 销毁

    // 子类可以重写这些事件处理函数来处理特定的消息
    virtual void OnKeyDown(UINT8 /*key*/) {} // 按键按下事件
    virtual void OnKeyUp(UINT8 /*key*/) {} // 按键释放事件

    // 访问器函数
    UINT GetWidth() const { return m_width; }  // 获取宽度
    UINT GetHeight() const { return m_height; } // 获取高度
    const WCHAR* GetTitle() const { return m_title.c_str(); } // 获取窗口标题

    // 解析命令行参数
    void ParseCommandLineArgs(_In_reads_(argc) WCHAR* argv[], int argc);

protected:
    // 获取资源的完整路径
    std::wstring GetAssetFullPath(LPCWSTR assetName);

    // 获取硬件适配器
    void GetHardwareAdapter(
        _In_ IDXGIFactory1* pFactory,
        _Outptr_result_maybenull_ IDXGIAdapter1** ppAdapter,
        bool requestHighPerformanceAdapter = false);

    // 设置自定义窗口文本
    void SetCustomWindowText(LPCWSTR text);

    // 视口尺寸
    UINT m_width;       // 宽度
    UINT m_height;      // 高度
    float m_aspectRatio; // 纵横比

    // 适配器信息
    bool m_useWarpDevice; // 是否使用WARP设备

private:
    // 根资源路径
    std::wstring m_assetsPath;

    // 窗口标题
    std::wstring m_title;
};
