#include "Pch.hpp"
#include "Json.hpp"

using namespace Json;

size_t Value::DefaultStringReserveLength = 32;

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
Value& Value::operator = (fat Value)
{
	Reset();
	type = Types::Integer;
	new (value)fat(Value);
	return *this;
}
Value& Value::operator = (double Value)
{
	Reset();
	type = Types::Floating;
	new (value) double(Value);
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
Value& Value::operator = (const Array& Value)
{
	Reset();
	type = Types::Array;
	new (value)Array(Value);
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
	case Types::Array:
		((Array*)(value))->~Array(); break;
	}
	type = Types::Invalid;
}

void Value::Read(std::istream& Stream)
{
	auto type = GuessType(Stream);
	char ch = 0;
	std::string s = "";

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

	case Types::Floating:
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
		s.reserve(DefaultStringReserveLength); //most strings are short
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
		Object properties;
		Stream.get(); //read {
		while (!Stream.eof() && Stream.peek() != '}')
		{
			SkipWhitespace(Stream);
			//skip comma
			if (Stream.peek() == ',')
			{
				Stream.get();
				SkipWhitespace(Stream);
			}

			//read key (supports single quotes)
			std::string key;
			key.reserve(32);

			char oq = Stream.get(), ch = Stream.get();
			while (ch != oq)
			{
				key += ch;
				if (ch == '\\')
					key += Stream.get();

				ch = Stream.get();
			}

			SkipWhitespace(Stream);
			Stream.get(); //read :
			SkipWhitespace(Stream);

			Value v;
			//type is guessed inside
			v.Read(Stream);
			properties[key] = v;

			SkipWhitespace(Stream);
		}
		SkipWhitespace(Stream);
		Stream.get(); //read }

		operator=(properties);
	}
	break;

	case Types::Array:
	{
		Array values;
		Stream.get(); //read [
		while (!Stream.eof() && Stream.peek() != ']')
		{
			SkipWhitespace(Stream);
			//skip comma
			if (Stream.peek() == ',')
			{
				Stream.get();
				SkipWhitespace(Stream);
			}

			Value v;
			//type is guessed inside
			v.Read(Stream);
			values.push_back(v);

			SkipWhitespace(Stream);
		}
		SkipWhitespace(Stream);
		Stream.get(); //read ]

		operator=(values);
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
		out = std::to_string(*(fat*)value);
		Stream.write(out.data(), out.length());
		break;

	case Types::Floating:
		out = std::to_string(*(double*)value);
		Stream.write(out.data(), out.length());
		break;

	case Types::String:
		Stream.put('"');
		Stream.write((*(std::string*)value).data(), (*(std::string*)value).length());
		Stream.put('"');
		break;

	case Types::Object:
	{
		Stream.put('{');

		auto& prop = *(Object*)value;

		size_t i = 0, n = prop.size();
		for (auto& p : prop)
		{
			i++;

			//write key
			Stream.put('"');
			Stream.write(p.first.data(), p.first.length());
			Stream.write("\":", 2);

			//write value
			p.second.Write(Stream);

			//write encloser
			if (i != n)
				Stream.put(',');
		}
		Stream.put('}');
	}
	break;

	case Types::Array:
	{
		Stream.put('[');

		auto& vec = *(Array*)value;

		size_t n = vec.size(), nm1 = n - 1;
		for (size_t i = 0; i < n; i++)
		{
			//write value
			vec[i].Write(Stream);

			//write encloser
			if (i != nm1)
				Stream.put(',');
		}
		Stream.put(']');
	}
	break;
	}
}


Types Value::GuessType(std::istream& Stream)
{
	char pk = Stream.peek();
	//figure out type of number (integer, floating)
	if ((pk >= '0' && pk <= '9') || pk == '-')
	{
		ptrdiff_t count = 1;
		char nk = Stream.get(); //skip first (already known from above)
		while ((nk = Stream.peek()) >= '0' && nk <= '9')
			count++, nk = Stream.get();

		Stream.seekg(-count, std::ios::cur); //reset

		if (nk == '.')
			return Types::Floating;
		return Types::Integer;
	}

	switch (pk)
	{
	case '{': return Types::Object;
	case '[': return Types::Array;
	case '.': return Types::Floating;
	case 't':
	case 'T':
	case 'f':
	case 'F': return Types::Boolean;
	case 'n':
	case 'N': return Types::Null;
	case '\'':
	case '"': return Types::String;

	default: return Types::Invalid;
	}
}

void Value::SkipWhitespace(std::istream& Stream)
{
	char pk;
	while (!Stream.eof() && ((pk = Stream.peek()) == ' ' || pk == '\t' || pk == '\r' || pk == '\n'))
		pk = Stream.get();
}
std::string Value::EscapeQuotes(std::string String)
{
	size_t loc = 0;
	while ((loc = String.find('"', loc)) != std::string::npos)
		String.replace(loc, 1, "\\\""), loc += 2;

	return String;
}