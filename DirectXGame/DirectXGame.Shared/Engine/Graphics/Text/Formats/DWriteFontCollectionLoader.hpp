#pragma once

#include "Pch.hpp"

//TODO: Support multiple fonts

//Font enumerator wrapper for a single font
class DWriteFontFileEnumerator : public IDWriteFontFileEnumerator
{
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		if (ppvObject == nullptr)
			return E_INVALIDARG;

		if (IsEqualIID(riid, __uuidof(IUnknown)))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}

		else if (IsEqualIID(riid, __uuidof(IDWriteFontFileEnumerator)))
		{
			*ppvObject = static_cast<IDWriteFontFileEnumerator*>(this);
			AddRef();
			return S_OK;
		}

		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef() { return static_cast<ULONG>(InterlockedIncrement(reinterpret_cast<LONG*>(&cRefCount))); }

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		ULONG newCount = static_cast<ULONG>(InterlockedDecrement(reinterpret_cast<LONG*>(&cRefCount)));

		if (newCount == 0)
			delete this;

		return newCount;
	}

	virtual HRESULT STDMETHODCALLTYPE GetCurrentFontFile(IDWriteFontFile **ppFontFile)
	{
		if (ppFontFile == nullptr)
			return E_INVALIDARG;

		if (pFontFile == nullptr)
			return E_FAIL;

		pFontFile->AddRef();
		*ppFontFile = pFontFile;

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL *pHasCurrentFile)
	{
		if (pHasCurrentFile == nullptr)
			return E_INVALIDARG;

		if (count > 0)
			*pHasCurrentFile = false;
		else
			*pHasCurrentFile = true;

		++count;

		return S_OK;
	}

	DWriteFontFileEnumerator(IDWriteFontFile *pFontFile) :
		cRefCount(1),
		pFontFile(pFontFile),
		count(0)
	{
		if (pFontFile)
			pFontFile->AddRef();
	}

private:
	~DWriteFontFileEnumerator()
	{
		if (pFontFile)
			pFontFile->Release();
	}

	ULONG cRefCount;
	IDWriteFontFile *pFontFile;
	UINT count;

	DWriteFontFileEnumerator(const DWriteFontFileEnumerator&);
	DWriteFontFileEnumerator& operator=(const DWriteFontFileEnumerator&);
};

//Font collection loader for a single font
class DWriteFontCollectionLoader : public IDWriteFontCollectionLoader
{
public:
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject)
	{
		if (ppvObject == nullptr)
			return E_INVALIDARG;

		if (IsEqualIID(riid, __uuidof(IUnknown)))
		{
			*ppvObject = static_cast<IUnknown*>(this);
			AddRef();
			return S_OK;
		}
		else if (IsEqualIID(riid, __uuidof(IDWriteFontCollectionLoader)))
		{
			*ppvObject = static_cast<IDWriteFontCollectionLoader*>(this);
			AddRef();
			return S_OK;
		}

		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef() { return static_cast<ULONG>(InterlockedIncrement(reinterpret_cast<LONG*>(&m_cRefCount))); }

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		ULONG newCount = static_cast<ULONG>(InterlockedDecrement(reinterpret_cast<LONG*>(&m_cRefCount)));

		if (newCount == 0)
			delete this;

		return newCount;
	}

	virtual HRESULT STDMETHODCALLTYPE CreateEnumeratorFromKey(
		IDWriteFactory *pFactory,
		const void *collectionKey,
		UINT32 collectionKeySize,
		IDWriteFontFileEnumerator **ppFontFileEnumerator
		)
	{
		collectionKey;
		collectionKeySize;

		if (pFactory == nullptr || ppFontFileEnumerator == nullptr)
			return E_INVALIDARG;

		IDWriteFontFile *pFontFile;
		HRESULT hResult = pFactory->CreateFontFileReference(filePath.c_str(), nullptr, &pFontFile);
		if (FAILED(hResult))
			return hResult;

		DWriteFontFileEnumerator *pEnum = new DWriteFontFileEnumerator(pFontFile);
		*ppFontFileEnumerator = pEnum;

		pFontFile->Release();

		return S_OK;
	}

	DWriteFontCollectionLoader(const WCHAR* FontfilePath) : m_cRefCount(1), filePath(FontfilePath) { }

private:
	~DWriteFontCollectionLoader() = default;

	ULONG m_cRefCount;
	std::wstring filePath;

	DWriteFontCollectionLoader(const DWriteFontCollectionLoader&) = default;
	DWriteFontCollectionLoader& operator=(const DWriteFontCollectionLoader&) = default;
};