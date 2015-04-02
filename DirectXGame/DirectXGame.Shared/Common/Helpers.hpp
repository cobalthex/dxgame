#pragma once

#include "Pch.hpp"

//return the size in bytes of the largest type in the pack
template<typename F, typename... Ts> struct SizeofLargestType { static const size_t size = (sizeof(F) > SizeofLargestType <Ts...>::size ? sizeof(F) : SizeofLargestType <Ts...>::size); };
template<typename F> struct SizeofLargestType <F> { static const size_t size = sizeof(F); };

//A simple anonymous (does not know type information) variant that can store a range of values defined by Ty
template <typename... Ty>
using VariantBuffer = uint8_t[SizeofLargestType<Ty...>::size];

inline std::wstring ToWString(const std::string& String)
{
	return std::wstring(String.begin(), String.end());
}

inline std::string StringToLower(std::string String)
{
	std::transform(String.begin(), String.end(), String.begin(), ::tolower);
	return String;
}
inline int StringICompare(const std::string& A, const std::string& B)
{
	return StringToLower(A).compare(StringToLower(B));
}
inline bool StringEndsWith(const std::string& String, const std::string& Suffix)
{
	if (Suffix.size() > String.size())
		return false;
	return std::equal(Suffix.rbegin(), Suffix.rend(), String.rbegin());
}
inline bool StringEndsWith(const std::wstring& String, const std::wstring& Suffix)
{
	if (Suffix.size() > String.size())
		return false;
	return std::equal(Suffix.rbegin(), Suffix.rend(), String.rbegin());
}


extern std::string StringReplace(std::string& String, const std::string& Find, const std::string& Replace);
extern std::wstring StringReplace(std::wstring& String, const std::wstring& Find, const std::wstring& Replace);
extern std::string StringReplace(std::string& String, const char* Find, const char* Replace);
extern std::wstring StringReplace(std::wstring& String, const wchar_t* Find, const wchar_t* Replace);

//Join two paths with a / if the end of A does not have one and the start of B does not have one
extern std::string CombinePaths(const std::string& PathA, const std::string& PathB);
extern std::wstring CombinePaths(const std::wstring& PathA, const std::wstring& PathB);
//Join a list of paths using the CombinePaths(A, B) function
template <typename T, typename... Args>
inline std::string CombinePaths(const T& Path, const Args&... Paths)
{
	return CombinePaths(Path, CombinePaths(Paths...));
}