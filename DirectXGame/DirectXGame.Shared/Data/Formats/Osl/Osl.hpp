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
		Object, //embedded objects do not have names (left empty)
		Reference, //A reference to another property (@object:name, or name if just @local)
		_ReferenceString, //A referencce to another property, temporarily stored as a string
	};

	class Value;

	typedef std::vector<Value> ValueList;
	
	//A single OSL object (Duplicate objects in a document are combined, both in properties and attributes; all else latter-replaced)
	class Object : public ISerializable
	{
	public:
		Object() : type(), attributes(), properties() { }

		std::string type; //the type of this object
		std::string name; //the name of this object
		std::set<std::string> attributes; //optional attributes (key only)
		std::map<std::string, ValueList> properties; //all properties in this object

		virtual void Read(std::istream& Stream) override; //Create a value from a stream
		virtual void Write(std::ostream& Stream) const override; //Write a value to a stream
	};

	typedef long long integer;
	typedef double decimal;
	typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> DateTime;
	typedef Object* Reference;

	//An OSL document
	class Document : public ISerializable
	{
		std::map<std::string, Object> objects;

		virtual void Read(std::istream& Stream) override; //Create a value from a stream
		virtual void Write(std::ostream& Stream) const override; //Write a value to a stream

		//Load a value automatically from a file
		static inline Document FromFile(const std::string& FileName)
		{
			std::ifstream fin(FileName, std::ios::in);
			Document d;
			d.Read(fin);
			fin.close();
			return d;
		}
	};

	//A value representing one of the OSL types (Note, all objects converted to date/time are times)
	class Value : public ISerializable
	{
	public:
		Value() : type(Types::Invalid) { }

		//factory methods

		template <typename T>
		static Value Create(const T& Val) { Value v; v.operator=(Val); return v; }

		static Value Create(const char* Val) { Value v; v.operator=(std::string(Val)); return v; } //Create a value from a string constant

		inline Types Type() const { return type; }

		//Set operators

		Value& operator = (const std::nullptr_t& Value);
		Value& operator = (bool Value);
		Value& operator = (integer Value);
		Value& operator = (decimal Value);
		Value& operator = (const std::string& Value);
		Value& operator = (const Object& Value);
		Value& operator = (const DateTime& Value);
		Value& operator = (const Reference& Value);

		inline Value& operator = (short Value) { return operator=(short(Value)); }
		inline Value& operator = (int Value) { return operator=(integer(Value)); }
		inline Value& operator = (unsigned Value) { return operator=(integer(Value)); }
		inline Value& operator = (float Value) { return operator=(float(Value)); }
		inline Value& operator = (const char* Value) { return operator=(std::string(Value)); }

		//Get operators

		template <typename T>
		inline operator T() const { return *(T*)(value); }

		//Serializers

		virtual void Read(std::istream& Stream) override; //Create a value from a stream
		virtual void Write(std::ostream& Stream) const override; //Write a value to a stream

		static size_t DefaultStringReserveLength; //the default string reservation length (in chars) - Defaults to 32
		static std::string WhitespaceCharacters; //characters that count as whitespace

	protected:
		Types type;
		Variant<std::nullptr_t, bool, integer, decimal, std::string, DateTime, Object, Reference> value;

		//Delete any old values and reset it to the default
		void Reset();

		static Types GuessType(std::istream& Stream); //Guess the type of object (Only looks based on the first character) - Assumes first character at stream pos is usable
		static void SkipWhitespace(std::istream& Stream); //Skip any whitespace (Schema defined whitespace)
		static std::string EscapeQuotes(std::string String);
	};
}