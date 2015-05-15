#pragma once

#include "Pch.hpp"

//Helper functions for streams
namespace StreamOps
{
	inline bool IsSpace(int Char) { return isspace(Char) ? true : false; } //The default IsSpace test function

	//Skip whitespace characters (defined by WhitespaceCmp). Returns the number of whitespace characters counted
	inline size_t SkipWhitespace(std::istream& Stream, const std::function<bool(int Char)>& WhitespaceCmp = IsSpace)
	{
		size_t sz = 0;
		while (!Stream.eof() && WhitespaceCmp(Stream.peek()))
		{
			sz++;
			Stream.get();
		}
		return sz;
	}

	//Read an entire file into a string (optionally in binary mode)
	inline std::string ReadFile(const std::string& File, bool IsBinary = true)
	{
		std::string str;
		std::ifstream input(File.data(), (IsBinary ? std::ios::binary : 0) | std::ios::ate);
		if (input.is_open())
		{
			str.resize(input.tellg());
			input.seekg(0, std::ios::beg);
			input.read(&str[0], str.size());
			input.close();
		}
		return str;
	}

	//Are the next characters in the stream the sequence?
	//If no, position is rewound, otherwise, true is returned and the stream is at oldpos + Sequence.length
	bool IsSequenceNext(std::istream& Stream, const std::string& Sequence);

	//Read the stream until Sequence is found
	std::string ReadUntil(std::istream& Stream, const std::string& Sequence);
	//ignore characters in the stream until Sequence is found
	size_t SkipUntil(std::istream& Stream, const std::string& Sequence);

	//Skip comments (typically in C style format) and leading whitespace
	//Returns the number of skipped characters
	//Note: requires that the comments start at the next character
	size_t SkipComments(std::istream& Stream, const std::string& SingleLine = "//", const std::string& MultiLineBegin = "/*", const std::string& MultiLineEnd = "*/");
}