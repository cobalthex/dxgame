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
		String, //Quoted strings or single word strings
		Date,
		Time,
		Object, //embedded objects do not have names or types (left empty)
		Reference, //A reference to another property (@object:name, or name if just @local)
		_ReferenceString, //A referencce to another property, temporarily stored as a string
		Type, //one of the types (mainly for schemas), start with *,
		Any  //Any type, does not have any value by itself
	};

	extern std::string WhitespaceCharacters; //characters that count as whitespace
	extern std::string TerminatorCharacters; //characters that count as word terminators
	inline std::string ReadWord(std::istream& Stream) //Read until hitting a whitespace or terminator character - returns a string
	{
		std::string s;
		char pk;
		while (
			!Stream.eof()
			&& WhitespaceCharacters.find(pk = Stream.peek()) == std::string::npos
			&& TerminatorCharacters.find(pk) == std::string::npos
			)
			s += Stream.get();
		return s;
	}
	inline void SkipWhitespace(std::istream& Stream) //Skip any whitespace (Schema defined whitespace)
	{
		while (!Stream.eof() && WhitespaceCharacters.find(Stream.peek()) != std::string::npos)
			Stream.get();
	}
	extern void SkipComments(std::istream& Stream);
	//skip comments then whitespace as many times as necessary until a non whitespace/comment is found
	inline void SkipNonValues(std::istream& Stream)
	{
		char pk;
		while (!Stream.eof() && (WhitespaceCharacters.find(pk = Stream.peek()) != std::string::npos || pk == '/'))
		{
			auto pos = Stream.tellg();

			SkipComments(Stream);
			SkipWhitespace(Stream);

			//nothing read, therefore going again isn't going to do anything
			if (Stream.tellg() == pos)
				break;
		}
	}

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

		inline ValueList& operator [](const std::string& Key) { return properties.at(Key); }
		inline const ValueList& operator [](const std::string& Key) const { return properties.at(Key); }

	protected:
		void ReadAttributes(std::istream& Stream); //Read in all attributes after the :

		friend class Value;
	};

	typedef long long integer;
	typedef double decimal;
	typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> Time;
	typedef Object* Reference;

	//An OSL document
	class Document : public ISerializable
	{
	public:
		std::map<std::string, Object> objects;

		virtual void Read(std::istream& Stream) override; //Create a value from a stream
		virtual void Write(std::ostream& Stream) const override; //Write a value to a stream

		inline void WriteToFile(const std::string& FileName)
		{
			std::ofstream fout(FileName, std::ios::out);
			Write(fout);
			fout.close();
		}

		//Load a value automatically from a file
		static inline Document FromFile(const std::string& FileName)
		{
			std::ifstream fin(FileName, std::ios::in);
			Document d;
			d.Read(fin);
			fin.close();
			return d;
		}

		inline Object& operator [](const std::string& Key) { return objects.at(Key); }
		inline const Object& operator [](const std::string& Key) const { return objects.at(Key); }

		void ConvertAllReferences(); //convert all reference strings to reference strings to pointers
	};

	//A simple representation of a date
	struct Date
	{
		int year : 2;
		int month : 1;
		int day : 1;

		Date() : year(0), month(0), day(0) { }
	};

	//A value representing one of the OSL types
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
		Value& operator = (const Date& Value);
		Value& operator = (const Time& Value);
		Value& operator = (const Reference& Value);
		Value& operator = (Types Value);

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

	protected:
		Types type;
		Variant<std::nullptr_t, bool, integer, decimal, std::string, Date, Time, Object, Reference, Types> value;

		//Delete any old values and reset it to the default
		void Reset();

		static std::string EscapeQuotes(std::string String);
		static Types GuessType(std::istream& Stream); //Guess the type of the value (Only looks based on the first character) - Assumes first character at stream pos is usable
	};
}