#include "Pch.hpp"
#include "Osl.hpp"
#include <ctime>
#include "Common/StringOps.hpp"

using namespace Osl;

std::string Osl::WhitespaceCharacters = "\n\r\f\t\v ";
std::string Osl::TerminatorCharacters = "{};";

void Osl::SkipComments(std::istream& Stream)
{
	if (Stream.peek() == '/')
	{
		Stream.get();
		char pk;
		if ((pk = Stream.peek()) == '/')
			while (!Stream.eof() && Stream.get() != '\n'); //line comment
		else if (pk == '*')
		{
			Stream.get();
			while (!Stream.eof())
			{
				if (Stream.get() == '*' && Stream.get() == '/')
					return;
			}
		}
	}
}

bool Object::Read(std::istream& Stream)
{
	//read first word
	SkipNonValues(Stream);
	char pk = Stream.peek();
	//if no name or type
	if (pk == ':')
	{
		Stream.get(); //skip :
		ReadAttributes(Stream);
	}
	//contains type and/or name
	if (pk != '{')
	{
		auto a = ReadWord(Stream);
		SkipNonValues(Stream);
		if (Stream.peek() != '{')
		{
			type = a;
			name = ReadWord(Stream);
		}
		else
			name = a;
		SkipNonValues(Stream);
		if (Stream.peek() == ':')
		{
			Stream.get();
			ReadAttributes(Stream);
		}
	}
	//read into object (already skipped whitespace above)
	Stream.get(); //skip {
	SkipNonValues(Stream);
	while ((pk = Stream.peek()) != '}')
	{
		if (Stream.eof())
			return false;

		SkipNonValues(Stream);

		//read to colon (will read the colon)
		std::string prop;
		while ((pk = Stream.get()) != ':')
		{
			if (Stream.eof())
				return false;
			prop += pk;
		}

		//trim comments from the property name
		static const char* commStr = "/*";
		prop.resize(std::find_end(prop.begin(), prop.end(), commStr, commStr + 2) - prop.begin()); //trim comments at end of string
		prop.resize(prop.find_last_not_of(WhitespaceCharacters) + 1); //trim end of string

		SkipNonValues(Stream);
		while ((pk = Stream.peek()) != ';' && (pk != '}'))
		{
			if (Stream.eof())
				return false;

			Value v;
			if (!v.Read(Stream))
				return false;
			properties[prop].push_back(v);
			SkipNonValues(Stream);
		}
		Stream.get(); //semicolon
		SkipNonValues(Stream);
	}
	Stream.get(); // }

	return true;
}

void Object::ReadAttributes(std::istream& Stream)
{
	SkipNonValues(Stream);
	while (!Stream.eof() && Stream.peek() != '{')
	{
		attributes.insert(ReadWord(Stream));
		SkipNonValues(Stream);
	}
}

bool Object::Write(std::ostream& Stream) const
{
	bool written = false; //for writing minimal string
	if (type.size() > 0)
	{
		Stream.write(type.data(), type.size());
		written = true;
	}
	if (name.size() > 0)
	{
		if (written)
			Stream.put(' ');
		Stream.write(name.data(), name.size());
		written = true;
	}
	if (attributes.size() > 0)
	{
		Stream.put(':');
		written = false;
		for (auto& attr : attributes)
		{
			if (written)
				Stream.put(' ');
			Stream.write(attr.data(), attr.size());
			written = true;
		}
	}
	Stream.put('{');

	for (auto& p : properties)
	{
		//write key
		Stream.write(p.first.data(), p.first.length());
		Stream.put(':');

		bool written = false;
		//write value
		for (auto& v : p.second)
		{
			if (written)
				Stream.put(' ');
			v.Write(Stream);
			written = true;
		}

		Stream.put(';');
	}
	Stream.put('}');

	return true;
}

bool Document::Read(std::istream& Stream)
{
	bool open = ((std::ifstream*)&Stream)->is_open();
	SkipWhitespace(Stream);

	while (!Stream.eof())
	{
		SkipNonValues(Stream);
		Object o;
		o.Read(Stream);
		objects[o.name] = o;
		SkipNonValues(Stream); //necessary to make sure not trailing empty space
	}

	return true;
}

bool Document::Write(std::ostream& Stream) const
{
	for (auto& o : objects)
		o.second.Write(Stream);

	return true;
}

void Document::ConvertAllReferences()
{
	std::stack<Object> s;
	for (auto& o : objects)
		s.push(o.second);

	while (!s.empty())
	{
		auto o = s.top();
		s.pop();
		for (auto& p : o.properties)
		{
			for (auto& v : p.second)
			{
				if (v.Type() == Types::_ReferenceString)
				{
					auto f = objects.find((std::string)v);
					v = (f != objects.end() ? &f->second : nullptr);
				}
				else if (v.Type() == Types::Object)
					s.push((Object)v);
			}
		}
	}
}

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
	new (value)decimal(Value);
	return *this;
}
Value& Value::operator = (const std::string& Value)
{
	Reset();
	type = Types::String;
	new (value)std::string(Value);
	return *this;
}
Value& Value::operator = (const Date& Value)
{
	Reset();
	type = Types::Date;
	new (value)Date(Value);
	return *this;
}
Value& Value::operator = (const Time& Value)
{
	Reset();
	type = Types::Time;
	new (value)Time(Value);
	return *this;
}
Value& Value::operator = (const Tuple& Value)
{
	Reset();
	type = Types::Tuple;
	new (value)Tuple(Value);
	return *this;
}
Value& Value::operator = (const Object& Value)
{
	Reset();
	type = Types::Object;
	new (value)Object(Value);
	return *this;
}
Value& Value::operator = (const Reference& Value)
{
	Reset();
	type = Types::Reference;
	new (value)Reference(Value);
	return *this;
}
Value& Value::operator = (Types Value)
{
	Reset();
	type = Types::Type;
	new (value)Types(Value);
	return *this;
}

void Value::Reset()
{
	switch (type)
	{
	case Types::String:
	case Types::_ReferenceString:
		((std::string*)(value))->~basic_string(); break;
	case Types::Tuple:
		((Tuple*)(value))->~Tuple(); break;
	case Types::Object:
		((Object*)(value))->~Object(); break;
	case Types::Date:
		((Date*)(value))->~Date(); break;
	case Types::Time:
		((Time*)(value))->~Time(); break;
	}
}

bool Value::Read(std::istream& Stream)
{
	auto ty = GuessType(Stream);
	char ch = 0;
	std::string s = "";

	switch (ty)
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
			if (Stream.eof())
				return false;

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
			if (Stream.eof())
				return false;

			s += ch;
			Stream.get();
		}

		operator=(std::stod(s));
		break;

	case Types::String:
	{
		char us[5]; //unicode string (first char is u)
		char lastUni; //last unicode character (for surrogate pairs)
		bool isLastUni = false; //was the last character a unicode character (for combining pairs)

		if ((ch = Stream.peek()) == '\'' || ch == '"')
		{
			char oq = Stream.get();
			ch = Stream.get(); //(supports single quotes)
			while (ch != oq)
			{
				if (Stream.eof())
					return false;

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
		else
			operator=(ReadWord(Stream)); //single word
	}
	break;

	case Types::Date:
	{
		Date d;

		while ((ch = Stream.peek()) >= '0' && ch <= '9')
		{
			if (Stream.eof())
				return false;
			s += Stream.get();
		}
		d.year = strtoul(s.data(), nullptr, 10);
		Stream.get();

		s.clear();
		while ((ch = Stream.peek()) >= '0' && ch <= '9')
		{
			if (Stream.eof())
				return false;
			s += Stream.get();
		}
		d.month = strtoul(s.data(), nullptr, 10);
		Stream.get();

		s.clear();
		while ((ch = Stream.peek()) >= '0' && ch <= '9')
		{
			if (Stream.eof())
				return false;
			s += Stream.get();
		}
		d.day = strtoul(s.data(), nullptr, 10);

		operator=(d);
	}
	break;

	case Types::Time:
	{
		while ((ch = Stream.peek()) >= '0' && ch <= '9')
		{
			if (Stream.eof())
				return false;
			s += Stream.get();
		}
		unsigned hr = strtoul(s.data(), nullptr, 10);
		Stream.get();

		s.clear();
		while ((ch = Stream.peek()) >= '0' && ch <= '9')
		{
			if (Stream.eof())
				return false;
			s += Stream.get();
		}
		unsigned min = strtoul(s.data(), nullptr, 10);
		Stream.get();

		s.clear();
		while ((ch = Stream.peek()) >= '0' && ch <= '9')
		{
			if (Stream.eof())
				return false;
			s += Stream.get();
		}
		unsigned sec = strtoul(s.data(), nullptr, 10);

		unsigned ms = 0;
		if ((ch = Stream.peek()) == '.')
		{
			Stream.get();

			s.clear();
			while ((ch = Stream.peek()) >= '0' && ch <= '9')
			{
				if (Stream.eof())
					return false;
				s += Stream.get();
			}
			ms = strtoul(s.data(), nullptr, 10);
		}

		Time t;
		t += std::chrono::hours(hr);
		t += std::chrono::minutes(min);
		t += std::chrono::seconds(sec);
		t += std::chrono::milliseconds(ms);

		operator=(t);
	}
	break;

	case Types::Tuple:
	{
		Tuple values;
		char ob = Stream.get(); //get opening bracket: ( or [
		SkipNonValues(Stream);
		while ((ch = Stream.peek()) != (ob == '(' ? ')' : ']') && ch != ';' && ch != '}')
		{
			if (Stream.eof())
				return false;

			Value v;
			if (!v.Read(Stream))
				return false;
			values.push_back(v);

			SkipNonValues(Stream);
		}
		ob = Stream.get(); //get ) or ]

		operator=(values);
	}
	break;

	case Types::Object:
	{
		Object o;
		if (!o.Read(Stream))
			return false;
		operator=(o);
	}
	break;

	case Types::_ReferenceString:
	{
		//References are parsed as strings and converted to pointers by Document (after all objects are loaded)
		Stream.get(); //skip @
		s = ReadWord(Stream);

		Reset();
		type = Types::_ReferenceString;
		new (value)std::string(s);
	}
	break;

	case Types::Type:
	{
		Stream.get(); //skip $
		s = ReadWord(Stream);
		StringOps::ToLower(s);
		Types t;

		if (s == "null" || s == "nil") t = Types::Null;
		else if (s == "bool" || s == "boolean") t = Types::Boolean;
		else if (s == "int" || s == "integer") t = Types::Integer;
		else if (s == "dec" || s == "decimal" || s == "float") t = Types::Decimal;
		else if (s == "str" || s == "string") t = Types::String;
		else if (s == "date") t = Types::Date;
		else if (s == "time") t = Types::Time;
		else if (s == "obj" || s == "object") t = Types::Object;
		else if (s == "ref" || s == "reference") t = Types::Reference;
		else if (s == "type") t = Types::Type;
		else if (s == "" || s == "any") t = Types::Any;

		operator=(t);
	}
	break;

	default:
		char pk;
		while (!Stream.eof() && WhitespaceCharacters.find(pk = Stream.peek()) == std::string::npos && TerminatorCharacters.find(pk) == std::string::npos)
			Stream.get();
		break;

	}
	return true;
}
bool Value::Write(std::ostream& Stream) const
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
		auto d = (Date*)value;
		char ds[12];
		auto sz = sprintf_s(ds, "%04i-%02i-%02i", d->year, d->month, d->day);
		Stream.write(ds, sz);
	}
	break;

	case Types::Time:
	{
		auto dur = *(Time*)value - Time();
		auto mil = dur.count() % 1000;
		auto sec = std::chrono::duration_cast<std::chrono::seconds>(dur).count() % 60;
		auto min = std::chrono::duration_cast<std::chrono::minutes>(dur).count() % 60;
		auto hour = std::chrono::duration_cast<std::chrono::hours>(dur).count();
		std::string t;
		t.reserve(32);
		t += std::to_string(hour);
		t += ':';
		if (min < 10) t += '0';
		t += std::to_string(min);
		t += ':';
		if (sec < 10) t += '0';
		t += std::to_string(sec);
		if (mil > 0)
		{
			t += '.';
			if (mil < 100) t += '0';
			if (mil < 10) t += '0';
			t += std::to_string(mil);
		}
		Stream.write(t.data(), t.size());
	}
	break;

	case Types::Tuple:
	{
		Stream.put('(');
		bool written = false;
		auto t = (Tuple*)value;
		auto sz = t->size();
		for (size_t i = 0; i < t->size(); i++)
		{
			if (written)
				Stream.put(' ');
			written = true;

			auto v = t->at(i);
			if (!v.Write(Stream))
				return false;
		}
		Stream.put(')');
	}
	break;

	case Types::Object:
		return ((Object*)value)->Write(Stream);

	case Types::Reference:
		Stream.put('@');
		Stream.write((*(Object**)value)->name.data(), (*(Object**)value)->name.length());
		break;

	case Types::_ReferenceString:
		Stream.put('@');
		Stream.write(((std::string*)value)->data(), ((std::string*)value)->length());
		break;

	case Types::Type:
		Stream.put('$');
		switch (*(Types*)value)
		{
		case Types::Null: Stream.write("null", 4); break;
		case Types::Boolean: Stream.write("boolean", 4); break;
		case Types::Integer: Stream.write("integer", 7); break;
		case Types::Decimal: Stream.write("decimal", 7); break;
		case Types::String: Stream.write("string", 6); break;
		case Types::Date: Stream.write("date", 4); break;
		case Types::Time: Stream.write("time", 4); break;
		case Types::Object: Stream.write("object", 6); break;
		case Types::_ReferenceString:
		case Types::Reference: Stream.write("reference", 9); break;
		case Types::Type: Stream.write("type", 4); break;
		case Types::Any: break;
		default: Stream.write("invalid", 7); break;
		}
		break;
	}
	return true;
}

std::string Value::EscapeQuotes(std::string String)
{
	size_t loc = 0;
	while ((loc = String.find('"', loc)) != std::string::npos)
		String.replace(loc, 1, "\\\""), loc += 2;

	return String;
}

Types Value::GuessType(std::istream& Stream)
{
	char pk = Stream.peek();

	//figure out type of number (integer, Decimal)
	if ((pk >= '0' && pk <= '9') || pk == '-')
	{
		ptrdiff_t count = 1;
		char nk = Stream.get(); //skip first (already known from above)
		while ((nk = Stream.peek()) >= '0' && nk <= '9')
			count++, nk = Stream.get();
		Stream.seekg(-count, std::ios::cur); //reset

		if (count == 4 && nk == '-')
			return Types::Date;
		if (nk == ':')
			return Types::Time;
		if (nk == '.')
			return Types::Decimal;
		return Types::Integer;
	}

	if (pk == '.')
		return Types::Decimal;

	if (pk == 't' || pk == 'T')
	{
		char str[4];
		Stream.read(str, 4);
		Stream.seekg(-4, std::ios::cur);
		for (auto i = str; i != str + 4; i++) *i = tolower(*i);
		if (strncmp("true", str, 4) == 0)
			return Types::Boolean;
		return Types::String;
	}
	if (pk == 'f' || pk == 'F')
	{
		char str[5];
		Stream.read(str, 5);
		Stream.seekg(-5, std::ios::cur);
		for (auto i = str; i != str + 5; i++) *i = tolower(*i);
		if (strncmp("false", str, 5) == 0)
			return Types::Boolean;
		return Types::String;
	}
	if (pk == 'n' || pk == 'N')
	{
		char str[4];
		Stream.read(str, 3);
		if (str[2] == 'u')
		{
			str[3] = Stream.get();
			Stream.seekg(-4, std::ios::cur);
			for (auto i = str; i != str + 4; i++) *i = tolower(*i);
			if (strncmp("null", str, 4) == 0)
				return Types::Null;
			else
				return Types::String;
		}
		else
		{
			Stream.seekg(-3, std::ios::cur);
			for (auto i = str; i != str + 4; i++) *i = tolower(*i);
			if (strncmp("nil", str, 4) == 0)
				return Types::Null;
			return Types::String;
		}
	}

	switch (pk)
	{
	case '\'':
	case '"': return Types::String;
	case '@': return Types::_ReferenceString;
	case '{': return Types::Object;
	case '[':
	case '(': return Types::Tuple;
	case '$': return Types::Type;

	default: return Types::String; //Defaults to string
	}
}