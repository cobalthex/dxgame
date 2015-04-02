#include "Pch.hpp"
#include "Helpers.hpp"

std::string StringReplace(std::string& String, const std::string& Find, const std::string& Replace)
{
	size_t pos = 0;
	while ((pos = String.find(Find, pos)) != std::string::npos)
	{
		String.replace(pos, Find.length(), Replace);
		pos += Replace.length();
	}
	return String;
}
std::wstring StringReplace(std::wstring& String, const std::wstring& Find, const std::wstring& Replace)
{
	size_t pos = 0;
	while ((pos = String.find(Find, pos)) != std::wstring::npos)
	{
		String.replace(pos, Find.length(), Replace);
		pos += Replace.length();
	}
	return String;
}
std::string StringReplace(std::string& String, const char* Find, const char* Replace)
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
std::wstring StringReplace(std::wstring& String, const wchar_t* Find, const wchar_t* Replace)
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
std::string CombinePaths(const std::string& PathA, const std::string& PathB)
{
	if (PathA.length() < 1)
		return PathB;
	if (PathB.length() < 1)
		return PathA;

	if (PathA.back() == '/' || PathB.front() == '/')
		return PathA + PathB;

	return PathA + '/' + PathB;
}
std::wstring CombinePaths(const std::wstring& PathA, const std::wstring& PathB)
{
	if (PathA.length() < 1)
		return PathB;
	if (PathB.length() < 1)
		return PathA;

	if (PathA.back() == L'/' || PathB.front() == L'/')
		return PathA + PathB;

	return PathA + L'/' + PathB;
}