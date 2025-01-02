#pragma once

#include "DXSample.h"

// ǰ������ DXSample ��
class DXSample;

// Win32Application �ඨ��
class Win32Application
{
public:
    // ����Ӧ�ó���
    static int Run(DXSample* pSample, HINSTANCE hInstance, int nCmdShow);
    // ��ȡ���ھ��
    static HWND GetHwnd() { return m_hwnd; }

protected:
    // ���ڹ��̻ص�����
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    // ���ھ��
    static HWND m_hwnd;
};

