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

	//reads an int from a stream. Returns 0 if an invalid read
	//Differs from istream's as it only reads until a non-numeric is found
	//Skips whitespace ahead of numeric
	//Does not handle digit grouping
	int ReadInt(std::istream& Stream, int DecimalSeparator = '.');

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