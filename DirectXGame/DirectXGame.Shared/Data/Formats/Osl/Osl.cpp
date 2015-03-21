#include "Pch.hpp"
#include "Osl.hpp"
#include <ctime>

using namespace Osl;


void Object::Read(std::istream& Stream)
{

}

void Object::Write(std::ostream& Stream) const
{
	if (type.size() > 0)
	{
		Stream.write(type.data(), type.size());
		Stream.put(' ');
	}
	if (name.size() > 0)
	{
		Stream.write(name.data(), name.size());
		Stream.put(' ');
	}
	if (attributes.size() > 0)
	{
		Stream.write(": ", 2);
		for (auto& attr : attributes)
		{
			Stream.write(attr.data(), attr.size());
			Stream.put(' ');
		}
	}
	Stream.put('{');

	for (auto& p : properties)
	{
		//write key
		Stream.put('"');
		Stream.write(p.first.data(), p.first.length());
		Stream.write("\":", 2);

		bool first = true;
		//write value
		for (auto& v : p.second)
		{
			Stream.put(' ');
			v.Write(Stream);
			first = false;
		}

		Stream.put(';');
	}
	Stream.put('}');
}

void Document::Read(std::istream& Stream)
{

}

void Document::Write(std::ostream& Stream) const
{

}

size_t Value::DefaultStringReserveLength = 32;
std::string Value::WhitespaceCharacters = "\n\r\t ";

Value& Value::operator = (const std::nullptr_t& Value)
{
	Reset();
	type = Types::Null;
	new (value)std::nullptr_t(Value);
	return *this;
}
Value& Value::operator = (bool Value)
{
	Reset();
	type = Types::Boolean;
	new (value) bool(Value);
	return *this;
}
Value& Value::operator = (integer Value)
{
	Reset();
	type = Types::Integer;
	new (value)integer(Value);
	return *this;
}
Value& Value::operator = (decimal Value)
{
	Reset();
	type = Types::Decimal;
	new (value) decimal(Value);
	return *this;
}
Value& Value::operator = (const std::string& Value)
{
	Reset();
	type = Types::String;
	new (value)std::string(Value);
	return *this;
}
Value& Value::operator = (const Object& Value)
{
	Reset();
	type = Types::Object;
	new (value)Object(Value);
	return *this;
}

Value& Value::operator = (const DateTime& Value)
{
	Reset();
	type = Types::Time;
	new (value)DateTime(Value);
	return *this;

}
Value& Value::operator = (const Reference& Value)
{
	Reset();
	type = Types::Reference;
	new (value)Reference(Value);
	return *this;
}

void Value::Reset()
{
	switch (type)
	{
	case Types::String:
		((std::string*)(value))->~basic_string(); break;
	case Types::Object:
		((Object*)(value))->~Object(); break;
	case Types::Date:
	case Types::Time:
		((DateTime*)(value))->~DateTime(); break;
	}
}

void Value::Read(std::istream& Stream)
{
	auto type = GuessType(Stream);
	char ch = 0;
	std::string s = "";
	s.reserve(DefaultStringReserveLength); //most strings are short

	switch (type)
	{
	case Types::Null:
		operator=(std::nullptr_t());
		Stream.seekg(4, std::ios::cur);
		break;

	case Types::Boolean:
		ch = Stream.get();
		if (ch == 't' || ch == 'T')
		{
			operator=(true);
			Stream.seekg(3, std::ios::cur);
		}
		else
		{
			operator=(false);
			Stream.seekg(4, std::ios::cur);
		}
		break;

	case Types::Integer:
		//negative must come first
		if (Stream.peek() == '-')
			s.append('-', 1);

		while (((ch = Stream.peek()) >= '0' && ch <= '9') || ch == 'e' || ch == 'E')
		{
			s += ch;
			Stream.get();
		}
		//if (s.length() > 0)
		operator=(std::stoll(s));
		break;

	case Types::Decimal:
		//negative must come first
		if (Stream.peek() == '-')
			s.append('-', 1);

		while (((ch = Stream.peek()) >= '0' && ch <= '9') || ch == 'e' || ch == 'E' || ch == '.')
		{
			s += ch;
			Stream.get();
		}

		operator=(std::stod(s));
		break;

	case Types::String:
	{
		char us[5]; //unicode string (first char is u)
		char lastUni; //last unicode character (for surrogate pairs)
		bool isLastUni = false;

		char oq = Stream.get();
		ch = Stream.get(); //(supports single quotes)
		while (ch != oq)
		{
			if (ch == '\\') //escape character
			{
				//parse unicode quads
				if ((ch = Stream.peek()) == 'u' || ch == 'U')
				{
					Stream.read(us, 5); //read uXXXX
					int u = std::stoi(us + 1, 0, 16);

					//surrogate pairs
					if (isLastUni)
					{
						s += (char)(u + lastUni);
						isLastUni = false;
					}
					else
					{
						lastUni = u;
						isLastUni = true;
					}
				}
				else
				{
					s += Stream.get();
					isLastUni = false;
				}
			}
			else //regular char
			{
				if (isLastUni)
				{
					s += lastUni;
					isLastUni = false;
				}

				s += ch;
			}

			ch = Stream.get();
		}
		//add last unicode
		if (isLastUni)
			s += lastUni;

		operator=(s);
	}
	break;

	case Types::Object:
	{
		Object o;
		o.Read(Stream);
		operator=(o);
	}
	break;

	case Types::Reference:
	{
		//References are parsed as strings and converted to pointers by Document (after all objects are loaded)
		Stream.get(); //skip @
		char pk;
		while (!Stream.eof() && WhitespaceCharacters.find(pk = Stream.peek()) >= 0 && pk != ';')
			s += Stream.get();

		type = Types::_ReferenceString;
		new (value)std::string(s);
	}
	break;

	}
}
void Value::Write(std::ostream& Stream) const
{
	std::string out;
	switch (type)
	{
	case Types::Null:
		Stream.write("null", 4);
		break;

	case Types::Boolean:
		out = (*(bool*)value == true ? "true" : "false");
		Stream.write(out.data(), out.length());
		break;

	case Types::Integer:
		out = std::to_string(*(integer*)value);
		Stream.write(out.data(), out.length());
		break;

	case Types::Decimal:
		out = std::to_string(*(decimal*)value);
		Stream.write(out.data(), out.length());
		break;

	case Types::String:
		Stream.put('"');
		Stream.write(((std::string*)value)->data(), ((std::string*)value)->length());
		Stream.put('"');
		break;

	case Types::Date:
	{
		auto t = std::chrono::system_clock::to_time_t(*((DateTime*)value));
		auto* tp = gmtime(&t);
		char ts[11];
		auto sz = strftime(ts, 11, "%F", tp);
		Stream.write(ts, sz);
	}
	break;

	case Types::Time:
		auto t = std::chrono::system_clock::to_time_t(*((DateTime*)value));
		auto* tp = gmtime(&t);
		char ts[24];
		auto sz = strftime(ts, 24, "%X", tp);
		Stream.write(ts, sz);
		break;

	case Types::Object:
		((Object*)value)->Write(Stream);
		break;

	case Types::Reference:
		Stream.put('@');
		Stream.write((*(Object**)value)->name.data(), (*(Object**)value)->name.size());
		break;

	case Types::_ReferenceString:
		Stream.put('@');
		Stream.write(((std::string*)value)->data(), ((std::string*)value)->length());
		break;
	}
}

Types Value::GuessType(std::istream& Stream)
{
	return Types::Invalid;
}

void Value::SkipWhitespace(std::istream& Stream)
{
	while (!Stream.eof() && WhitespaceCharacters.find(Stream.peek()) >= 0)
		Stream.get();
}
std::string Value::EscapeQuotes(std::string String)
{
	size_t loc = 0;
	while ((loc = String.find('"', loc)) != std::string::npos)
		String.replace(loc, 1, "\\\""), loc += 2;

	return String;
}