#pragma once
#include <stdexcept>
#include <windows.h>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>
#include <d3dx12.h>

// 注意，虽然 ComPtr 用于管理 CPU 上资源的生命周期，
// 但它不了解 GPU 上资源的生命周期。应用程序必须考虑
// GPU 资源的生命周期，以避免销毁可能仍被 GPU 引用的对象。
using Microsoft::WRL::ComPtr;

// 将 HRESULT 转换为字符串
inline std::string HrToString(HRESULT hr)
{
	char s_str[64] = {};
	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	return std::string(s_str);
}

// HrException 类，用于处理 HRESULT 错误
class HrException : public std::runtime_error
{
public:
	HrException(HRESULT hr, const std::string& message = "")
		: std::runtime_error(HrToString(hr) + (message.empty() ? "" : ("\n" + message))), m_hr(hr), m_message(message) {}

	HRESULT Error() const { return m_hr; }
	const std::string& Message() const { return m_message; }

private:
	const HRESULT m_hr;
	const std::string m_message;
};

// 安全释放宏
#define SAFE_RELEASE(p) if (p) (p)->Release()

// 如果 HRESULT 失败，则抛出异常
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw HrException(hr);
	}
}

// 获取资源路径
inline void GetAssetsPath(_Out_writes_(pathSize) WCHAR* path, UINT pathSize)
{
	if (path == nullptr)
	{
		throw std::exception();
	}

	DWORD size = GetModuleFileName(nullptr, path, pathSize);
	if (size == 0 || size == pathSize)
	{
		// 方法失败或路径被截断。
		throw std::exception();
	}

	WCHAR* lastSlash = wcsrchr(path, L'\\');
	if (lastSlash)
	{
		*(lastSlash + 1) = L'\0';
	}
}


using namespace Microsoft::WRL::Wrappers;

inline HRESULT ReadDataFromFile(LPCWSTR filename, byte** data, UINT* size)
{
#if WINVER >= _WIN32_WINNT_WIN8
    CREATEFILE2_EXTENDED_PARAMETERS extendedParams = {};
    extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
    extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
    extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
    extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
    extendedParams.lpSecurityAttributes = nullptr;
    extendedParams.hTemplateFile = nullptr;

    FileHandle file(CreateFile2(filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &extendedParams));
#else
    FileHandle file(CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN | SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS, nullptr));
#endif
    if (file.Get() == INVALID_HANDLE_VALUE)
    {
        throw std::exception();
    }

    FILE_STANDARD_INFO fileInfo = {};
    if (!GetFileInformationByHandleEx(file.Get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
    {
        throw std::exception();
    }

    if (fileInfo.EndOfFile.HighPart != 0)
    {
        throw std::exception();
    }

    *data = reinterpret_cast<byte*>(malloc(fileInfo.EndOfFile.LowPart));
    *size = fileInfo.EndOfFile.LowPart;

    if (!ReadFile(file.Get(), *data, fileInfo.EndOfFile.LowPart, nullptr, nullptr))
    {
        throw std::exception();
    }

    return S_OK;
}

// 从 DDS 文件中读取数据
inline HRESULT ReadDataFromDDSFile(LPCWSTR filename, byte** data, UINT* offset, UINT* size)
{
	if (FAILED(ReadDataFromFile(filename, data, size)))
	{
		return E_FAIL;
	}

	// DDS 文件总是以相同的魔术数字开头。
	static const UINT DDS_MAGIC = 0x20534444;
	UINT magicNumber = *reinterpret_cast<const UINT*>(*data);
	if (magicNumber != DDS_MAGIC)
	{
		return E_FAIL;
	}

	struct DDS_PIXELFORMAT
	{
		UINT size;
		UINT flags;
		UINT fourCC;
		UINT rgbBitCount;
		UINT rBitMask;
		UINT gBitMask;
		UINT bBitMask;
		UINT aBitMask;
	};

	struct DDS_HEADER
	{
		UINT size;
		UINT flags;
		UINT height;
		UINT width;
		UINT pitchOrLinearSize;
		UINT depth;
		UINT mipMapCount;
		UINT reserved1[11];
		DDS_PIXELFORMAT ddsPixelFormat;
		UINT caps;
		UINT caps2;
		UINT caps3;
		UINT caps4;
		UINT reserved2;
	};

	auto ddsHeader = reinterpret_cast<const DDS_HEADER*>(*data + sizeof(UINT));
	if (ddsHeader->size != sizeof(DDS_HEADER) || ddsHeader->ddsPixelFormat.size != sizeof(DDS_PIXELFORMAT))
	{
		return E_FAIL;
	}

	const ptrdiff_t ddsDataOffset = sizeof(UINT) + sizeof(DDS_HEADER);
	*offset = ddsDataOffset;
	*size = *size - ddsDataOffset;

	return S_OK;
}

// 为对象分配名称以帮助调试。
#if defined(_DEBUG) || defined(DBG)
inline void SetName(ID3D12Object* pObject, LPCWSTR name)
{
	pObject->SetName(name);
}
inline void SetNameIndexed(ID3D12Object* pObject, LPCWSTR name, UINT index)
{
	WCHAR fullName[50];
	if (swprintf_s(fullName, L"%s[%u]", name, index) > 0)
	{
		pObject->SetName(fullName);
	}
}
#else
inline void SetName(ID3D12Object*, LPCWSTR)
{
}
inline void SetNameIndexed(ID3D12Object*, LPCWSTR, UINT)
{
}
#endif

// ComPtr<T> 的命名助手。
// 将变量的名称分配为对象的名称。
// 索引变体将在对象名称中包含索引。
#define NAME_D3D12_OBJECT(x) SetName((x).Get(), L#x)
#define NAME_D3D12_OBJECT_INDEXED(x, n) SetNameIndexed((x)[n].Get(), L#x, n)

// 计算常量缓冲区字节大小
inline UINT CalculateConstantBufferByteSize(UINT byteSize)
{
	// 常量缓冲区大小需要对齐。
	return (byteSize + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
}

#ifdef D3D_COMPILE_STANDARD_FILE_INCLUDE
// 编译着色器
inline Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const std::string& entrypoint,
	const std::string& target)
{
	UINT compileFlags = 0;
#if defined(_DEBUG) || defined(DBG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr;

	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	return byteCode;
}
#endif

// 重置 ComPtr 数组中的所有元素。
template<class T>
void ResetComPtrArray(T* comPtrArray)
{
	for (auto& i : *comPtrArray)
	{
		i.Reset();
	}
}

// 重置 unique_ptr 数组中的所有元素。
template<class T>
void ResetUniquePtrArray(T* uniquePtrArray)
{
	for (auto& i : *uniquePtrArray)
	{
		i.reset();
	}
}
