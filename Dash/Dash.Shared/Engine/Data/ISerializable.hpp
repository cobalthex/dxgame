#pragma once

#include "Pch.hpp"

//Provides basic input/output guarentees for serializablility
struct ISerializable
{
	virtual bool Read(std::istream& Stream) = 0; //Read from a stream
	virtual bool Write(std::ostream& Stream) const = 0; //Write to a stream
};

inline std::istream& operator >> (std::istream& Stream, ISerializable& Value) { Value.Read(Stream); return Stream; }
inline std::ostream& operator << (std::ostream& Stream, const ISerializable& Value) { Value.Write(Stream); return Stream; }