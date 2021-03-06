#include "Pch.hpp"
#include "StreamOps.hpp"

bool StreamOps::IsSequenceNext(std::istream& Stream, const std::string& Sequence)
{
	for (size_t i = 0; !Stream.eof() && i < Sequence.length(); i++)
	{
		if (Stream.peek() != Sequence[i])
		{
			Stream.seekg(-(ptrdiff_t)i, std::ios::cur);
			return false;
		}
		Stream.get();
	}
	return true;
}

std::string StreamOps::ReadUntil(std::istream& Stream, const std::string& Sequence)
{
	std::string s;
	while (!Stream.eof())
	{
		size_t i;
		for (i = 0; !Stream.eof() && i < Sequence.length(); i++)
		{
			s += Stream.get();
			if (s.back() != Sequence[i])
				break;
		}
		if (i == Sequence.length())
		{
			s.resize(s.length() - i);
			Stream.seekg(-(ptrdiff_t)i, std::ios::cur);
			return s;
		}
	}
	return s;
}

size_t StreamOps::SkipUntil(std::istream& Stream, const std::string& Sequence)
{
	size_t sz = 0;
	while (!Stream.eof())
	{
		size_t i;
		for (i = 0; !Stream.eof() && i < Sequence.length(); i++)
		{
			sz++;
			if (Stream.get() != Sequence[i])
				break;
		}
		if (i == Sequence.length())
		{
			Stream.seekg(-(ptrdiff_t)i, std::ios::cur);
			return sz - i;
		}
	}
	return sz;
}

size_t StreamOps::SkipComments(std::istream& Stream, const std::string& SingleLine, const std::string& MultiLineBegin, const std::string& MultiLineEnd)
{
	if (Stream.eof())
		return 0;

	if (IsSequenceNext(Stream, SingleLine))
	{
		std::string x;
		std::getline(Stream, x);
		return SingleLine.length() + x.length();
	}
	else if (IsSequenceNext(Stream, MultiLineBegin))
		return MultiLineBegin.length() + SkipUntil(Stream, MultiLineEnd);

	return 0;
}