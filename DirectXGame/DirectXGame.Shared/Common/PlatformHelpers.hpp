#pragma once

#include <ppltasks.h>	// For create_task

namespace App
{
	struct HandleCloserFunc { void operator()(HANDLE H) { if (H) CloseHandle(H); } }; //Close a handle with the () operator
	typedef std::unique_ptr<void, HandleCloserFunc> ScopedHandle; //A handle that automatically closes itself when it goes out of scope
	inline HANDLE SafeHandle(HANDLE H) { return (H == INVALID_HANDLE_VALUE) ? 0 : H; } //Make sure that this handle is valid

	//Get this application's working directory
	inline std::wstring GetWorkingDirectory()
	{
		return std::wstring(Windows::ApplicationModel::Package::Current->InstalledLocation->Path->Begin());
	}

	//Throw an exception based on the HRESULT if the operation failed
	inline void ThrowIfFailed(HRESULT Hr)
	{
		if (FAILED(Hr))
		{
			// Set a breakpoint on this line to catch Win32 API errors.
			throw Platform::Exception::CreateException(Hr);
		}
	}

	// Function that reads from a binary file asynchronously.
	inline Concurrency::task<std::vector<byte>> ReadDataAsync(const std::wstring& Filename)
	{
		using namespace Windows::Storage;
		using namespace Concurrency;

		auto folder = Windows::ApplicationModel::Package::Current->InstalledLocation;

		return create_task(folder->GetFileAsync(Platform::StringReference(Filename.c_str()))).then([](StorageFile^ file)
		{
			return FileIO::ReadBufferAsync(file);
		}).then([](Streams::IBuffer^ fileBuffer) -> std::vector < byte >
		{
			std::vector<byte> returnBuffer;
			returnBuffer.resize(fileBuffer->Length);
			Streams::DataReader::FromBuffer(fileBuffer)->ReadBytes(Platform::ArrayReference<byte>(returnBuffer.data(), fileBuffer->Length));
			return returnBuffer;
		});
	}

	// Helper for output debug tracing
	inline void DebugTrace(_In_z_ _Printf_format_string_ const char* format, ...)
	{
#ifdef _DEBUG
		va_list args;
		va_start(args, format);

		char buff[1024] = { 0 };
		vsprintf_s(buff, format, args);
		OutputDebugStringA(buff);
		va_end(args);
#else
		UNREFERENCED_PARAMETER(format);
#endif
	}


	// Converts a length in device-independent pixels (DIPs) to a length in physical pixels.
	inline float ConvertDipsToPixels(float Dips, float Dpi)
	{
		static const float dipsPerInch = 96.0f;
		return floorf(Dips * Dpi / dipsPerInch + 0.5f); // Round to nearest integer.
	}
}

namespace DX
{
#if defined(_DEBUG)
	// Check for SDK Layer support.
	inline bool SdkLayersAvailable()
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
			0,
			D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
			nullptr,                    // Any feature level will do.
			0,
			D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
			nullptr,                    // No need to keep the D3D device reference.
			nullptr,                    // No need to know the feature level.
			nullptr                     // No need to keep the D3D device context reference.
			);

		return SUCCEEDED(hr);
	}
#endif

	//Report the number of live objects in use by D3D11
	inline void ReportLiveObjects(ID3D11Device* device)
	{
		if (device)
		{
			ID3D11Debug* debugDev;
			device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDev));
			debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		}
	}

	//Helper sets a D3D resource name string (used by PIX and debug layer leak reporting).
	template<UINT TNameLength>
	inline void SetDebugObjectName(_In_ ID3D11DeviceChild* resource, _In_z_ const char(&name)[TNameLength])
	{
#if !defined(NO_D3D11_DEBUG_NAME) && ( defined(_DEBUG) || defined(PROFILE) )
#if defined(_XBOX_ONE) && defined(_TITLE)
		WCHAR wname[MAX_PATH];
		int result = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, name, TNameLength, wname, MAX_PATH);
		if (result > 0)
		{
			resource->SetName(wname);
		}
#else
		resource->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
#endif
#else
		UNREFERENCED_PARAMETER(resource);
		UNREFERENCED_PARAMETER(name);
#endif
	}
}
