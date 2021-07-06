#include "Parser.h"

Parser::Parser(Value& value, const std::string& content)
	:value_(value), current_(content.c_str())
{
	this->value_.set_type(kType::Null);
	this->parse_whitespace();
	this->parse_value();
	this->parse_whitespace();
	if ((*this->current_) != '\0')
	{
		this->value_.set_type(kType::Null);
		throw(JsonException("parse root not singular"));
	}
}

void Parser::parse_whitespace() noexcept
{
	while (*current_ == ' ' || *current_ == '\t' || *current_ == '\n' || *current_ == '\r')
		++current_;
}

void Parser::parse_value()
{
	switch (*current_)
	{
	case 'n': 
		this->parse_literal("null", kType::Null);
		return;
	case 't':
		this->parse_literal("true", kType::True);
		return;
	case 'f':
		this->parse_literal("false", kType::False);
		return;
	case '\"':
		this->parse_string(); 
		return;
	case '[':
		this->parse_array();
		return;
	case '{':
		this->parse_object();
		return;
	case '\0':
		throw(JsonException("parse expect value, please. "));
	default:
		this->parse_number();
		return;
	
	}
}

void Parser::parse_literal(const char* literal, kType t)
{
	expect(this->current_, literal[0]);
	size_t i = 1;
	for (;literal[i] != '\0'; i++)
	{
		if (this->current_[i] != literal[i])
		{
			throw(JsonException("parse invalid value"));
		}
	}
	this->current_ += i - 1;
	this->value_.set_type(t);
}

void Parser::parse_number()
{
	//used to check the fomate of the number
	//while finished, p pointed to the new position.
	const char* p = this->current_;
	if (*p == '-') ++p;
	
	if (*p == '0') ++p;
	else
	{
		if (!isdigit(*p)) throw(JsonException("parse invalid value. "));
		while (isdigit(*p)) ++p;
	}

	if (*p == '.') 
	{
		if (!isdigit(*++p)) throw(JsonException("parse invalid value. "));
		while (isdigit(*p)) ++p;
	}

	if (*p == 'e' || *p == 'E')
	{
		++p;
		if (*p == '+' || *p == '-') ++p;
		if (!isdigit(*p)) throw(JsonException("parse invalid value"));
		while (isdigit(*p)) p++;
	}
	
	//use strtod function to convert the number, and finished the latest check.
	errno = 0;
	double value = strtod(this->current_, NULL);
	if (errno == ERANGE && (value == HUGE_VAL || value == -HUGE_VAL))
		throw(JsonException("parse number too big. "));
	
	this->value_.set_number(value);
	this->current_ = p;
}

void Parser::parse_string()
{
	std::string str;
	parse_string_raw(str);
	this->value_.set_string(str);
}

void Parser::parse_string_raw(std::string& string)
{
	//for json presenting formate is string, so the " is convert sematic
	expect(this->current_, '\"');
	const char* p = this->current_;
	unsigned unicode = 0, unicode1 = 0;

	while (*p != '\"')
	{
		if (*p == '\0') throw(JsonException("parse miss quotation mark. "));
		
		if (*p == '\\' && *++p)
		{
			switch (*p++)
			{
			case '\"':
				string += '\"';
				break;
			case '\\':
				string += '\\';
				break;
			case 't':
				string += '\t';
				break;
			case 'b':
				string += '\b';
				break;
			case 'f':
				string += '\f';
				break;
			case 'n':
				string += '\n';
				break;
			case 'r':
				string += '\r';
				break;
			case 'u':
				parse_hex4(p, unicode);
				if (unicode >= 0xD800 && unicode <= 0xDBFF)
				{
					if (*p++ != '\\')
						throw(JsonException("parse invalid unicode surrogate. "));
					if (*p++ != 'u')
						throw(JsonException("parse invalid unicode surrogate. "));
					parse_hex4(p, unicode1);

					if (unicode1 < 0xDC00 || unicode1 > 0xDFFF)
						throw(JsonException("parse invalid unicode surrogate. "));

					unicode = (((unicode - 0xD800) << 10) | (unicode1 - 0xDC00)) + 0x10000;
				}
				parse_encode_utf8(string, unicode);
				break;
			default:
				throw(JsonException("parse invalid string escape. "));
			}
		}
		else if ((unsigned char)*p < 0x20)
			throw(JsonException("parse invalid string char. "));
		else
			string += *p++;
	}

	this->current_ = ++p;

}
