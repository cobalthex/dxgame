#include "Pch.hpp"
#include "StringOps.hpp"

std::string StringOps::Replace(std::string& String, const std::string& Find, const std::string& Replace)
{
	size_t pos = 0;
	while ((pos = String.find(Find, pos)) != std::string::npos)
	{
		String.replace(pos, Find.length(), Replace);
		pos += Replace.length();
	}
	return String;
}
std::wstring StringOps::Replace(std::wstring& String, const std::wstring& Find, const std::wstring& Replace)
{
	size_t pos = 0;
	while ((pos = String.find(Find, pos)) != std::wstring::npos)
	{
		String.replace(pos, Find.length(), Replace);
		pos += Replace.length();
	}
	return String;
}
std::string StringOps::Replace(std::string& String, const char* Find, const char* Replace)
{
	auto findLen = strlen(Find);
	auto repLen = strlen(Replace);
	size_t pos = 0;
	while ((pos = String.find(Find, pos)) != std::wstring::npos)
	{
		String.replace(pos, findLen, Replace);
		pos += repLen;
	}
	return String;
}
std::wstring StringOps::Replace(std::wstring& String, const wchar_t* Find, const wchar_t* Replace)
{
	auto findLen = wcslen(Find);
	auto repLen = wcslen(Replace);
	size_t pos = 0;
	while ((pos = String.find(Find, pos)) != std::string::npos)
	{
		String.replace(pos, findLen, Replace);
		pos += repLen;
	}
	return String;
}

//Join two paths with a / if the end of A does not have one and the start of B does not have one
std::string StringOps::CombinePaths(const std::string& PathA, const std::string& PathB, char CombineChar)
{
	if (PathA.length() < 1)
		return PathB;
	if (PathB.length() < 1)
		return PathA;

	if (PathA.back() == CombineChar || PathB.front() == CombineChar)
		return PathA + PathB;

	return PathA + CombineChar + PathB;
}
std::wstring StringOps::CombinePaths(const std::wstring& PathA, const std::wstring& PathB, wchar_t CombineChar)
{
	if (PathA.length() < 1)
		return PathB;
	if (PathB.length() < 1)
		return PathA;

	if (PathA.back() == CombineChar || PathB.front() == CombineChar)
		return PathA + PathB;

	return PathA + CombineChar + PathB;
}