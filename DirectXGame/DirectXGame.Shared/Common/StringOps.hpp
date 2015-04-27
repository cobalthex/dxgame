#pragma once

#include "Pch.hpp"

//Various operations that can be performed on strings
namespace StringOps
{
	inline std::wstring ToWString(const std::string& String)
	{
		return std::wstring(String.begin(), String.end());
	}

	inline std::string ToLower(std::string String)
	{
		std::transform(String.begin(), String.end(), String.begin(), ::tolower);
		return String;
	}
	//Case insensitive compare
	inline int ICompare(const std::string& A, const std::string& B)
	{
		return ToLower(A).compare(ToLower(B));
	}
	inline bool EndsWith(const std::string& String, const std::string& Suffix)
	{
		if (Suffix.size() > String.size())
			return false;
		return std::equal(Suffix.rbegin(), Suffix.rend(), String.rbegin());
	}
	inline bool EndsWith(const std::wstring& String, const std::wstring& Suffix)
	{
		if (Suffix.size() > String.size())
			return false;
		return std::equal(Suffix.rbegin(), Suffix.rend(), String.rbegin());
	}


	extern std::string Replace(std::string& String, const std::string& Find, const std::string& Replace);
	extern std::wstring Replace(std::wstring& String, const std::wstring& Find, const std::wstring& Replace);
	extern std::string Replace(std::string& String, const char* Find, const char* Replace);
	extern std::wstring Replace(std::wstring& String, const wchar_t* Find, const wchar_t* Replace);

	//Join two paths with a / if the end of A does not have one and the start of B does not have one
	extern std::string CombinePaths(const std::string& PathA, const std::string& PathB, char CombineChar = '/');
	extern std::wstring CombinePaths(const std::wstring& PathA, const std::wstring& PathB, wchar_t CombineChar = L'/');
	//Join a list of paths using the CombinePaths(A, B) function. Uses the default path char
	template <typename T, typename... Args>
	inline std::string CombinePaths(const T& Path, const Args&... Paths)
	{
		return CombinePaths(Path, CombinePaths(Paths...));
	}
}