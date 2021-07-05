#include "Value.h"
#include "Parser.h"
#include <cassert>

void Value::parse(const std::string& content)
{
	Parser(*this, content);
}

void Value::stringify(std::string& content) const noexcept
{
	if (this->type_ == kType::String && this->string_ != content)
	{
		content = string_;
	}
}

kType Value::get_type() const noexcept
{
	return this->type_;
}

void Value::set_type(kType type) noexcept
{
	free();
	this->type_ = type;
}

double Value::get_number() const noexcept
{
	assert(this->type_ == kType::Number);
	return this->number_;
}

void Value::set_number(double number) noexcept
{
	free();
	this->type_ = kType::Number;
	this->number_ = number;
}

const std::string& Value::get_string() const noexcept
{ 
	assert(this->type_ == kType::String);
	return this->string_;
}

void Value::set_string(const std::string& string) noexcept
{
	if (this->type_ == kType::String)
	{
		this->string_ = string;
	}
	else
	{
		free();
		this->type_ = kType::String;
		this->string_ = string;
	}
}

void Value::init(const Value& value) noexcept
{
	this->type_ = value.type_;
	this->number_ = 0;
	switch (this->type_)
	{
	case Number:
		this->number_ = value.number_;
		break;
	case String://copy on write
		this->string_ = value.string_;
		break;
	case Array:
		this->array_ = value.array_;
		break;
	case Object:
		this->object_ = value.object_;
		break;
	default:
		break;
	}
}

void Value::free() noexcept
{
	using namespace std;
	switch (this->type_)
	{
	case String:
		string_.~string();
		break;
	case Array:
		this->array_.~vector<Value>();
		break;
	case Object:
		this->object_.~unordered_map<string, Value>();
		break;
	default:
		break;
	}
}
