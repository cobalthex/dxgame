#pragma once

#include "Common/Helpers.hpp"
#include "Data/ISerializable.hpp"

//Basic OSL (Object serialization Language) IO. Limited Error handling. Supports all standard OSL types
namespace Osl
{
	//All of the OSL types
	enum class Types
	{
		Invalid,
		Null,
		Boolean,
		Integer,
		Decimal,
		String,
		Date,
		Time,
		Type, //A type name (all objects have types, all types must start with letter or _)
		Reference //A reference to another value (@object:name, or name if just @local)
	};

	class Value;

	typedef long long integer;
	typedef double decimal;

	typedef std::vector<Value> PropertyList;
	
	//A single OSL object
	class Object
	{
	public:
		std::string type;
		std::set<std::string> attributes; //optional attributes (key only)
		std::map<std::string, PropertyList> values;
	};

	//An OSL document
	class Document
	{
		std::map<std::string*, Object> objects;
	};

	//A value representing one of the OSL types. (integers floats, null, bool, strings, objects)
	class Value : public ISerializable
	{
	public:
		Value() : type(Types::Invalid) { }

		//factory methods

		template <typename T> static Value Create(const T& Val) { return Value(); }
		template <> static Value Create<nullptr_t>(const nullptr_t& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<bool>(const bool& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<integer>(const integer& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<decimal>(const decimal& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<std::string>(const std::string& Val) { Value v; v.operator=(Val); return v; }
		template <> static Value Create<Object>(const Object& Val) { Value v; v.operator=(Val); return v; }

		static Value Create(const char* Val) { Value v; v.operator=(std::string(Val)); return v; } //Create a value from a string constant

		inline Types Type() const { return type; }

		//Set operators

		Value& operator = (const std::nullptr_t& Value);
		Value& operator = (bool Value);
		Value& operator = (integer Value);
		Value& operator = (decimal Value);
		Value& operator = (const std::string& Value);
		Value& operator = (const Object& Value);

		inline Value& operator = (short Value) { return operator=(short(Value)); }
		inline Value& operator = (int Value) { return operator=(integer(Value)); }
		inline Value& operator = (unsigned Value) { return operator=(integer(Value)); }
		inline Value& operator = (float Value) { return operator=(float(Value)); }
		inline Value& operator = (const char* Value) { return operator=(std::string(Value)); }

		//Get operators

		inline operator std::nullptr_t() const { return *(std::nullptr_t*)(value); }
		inline operator bool() const { return *(bool*)(value); }
		inline operator integer () const { return *(integer*)(value); }
		inline operator decimal() const { return *(decimal*)(value); }
		inline operator std::string() const { return *(std::string*)(value); }
		inline operator Object() const { return *(Object*)(value); }

		inline operator short() const { return (short)(*(integer*)(value)); }
		inline operator int() const { return (int)(*(integer*)(value)); }
		inline operator unsigned() const { return (unsigned)(*(integer*)(value)); }
		inline operator float() const { return (float)(*(decimal*)(value)); }
		inline operator const char*() const { return ((std::string*)(value))->c_str(); }

		//Serializers

		void Read(std::istream& Stream); //Create a value from a stream
		void Write(std::ostream& Stream) const; //Write a value to a stream

		static size_t DefaultStringReserveLength; //the default string reservation length (in chars) - Defaults to 32

		//Helper methods

		//Load a value automatically from a file
		static inline Value FromFile(const std::string& FileName)
		{
			std::ifstream fin(FileName, std::ios::in);
			
			Value v;

			fin.close();
			return v;
		}

	protected:
		Types type;
		Variant<std::nullptr_t, bool, integer, decimal, std::string, Object> value;

		//Delete any old values and reset it to the default
		void Reset();

		static Types GuessType(std::istream& Stream); //Guess the type of object (Only looks based on the first character) - Assumes first character at stream pos is usable
		static void SkipWhitespace(std::istream& Stream); //Skip any whitespace (Schema defined whitespace)
		static std::string EscapeQuotes(std::string String);
	};
}