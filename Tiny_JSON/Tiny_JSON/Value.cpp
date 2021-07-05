#include "Value.h"
#include "Parser.h"
#include <cassert>

void Value::parse(const std::string& content)
{
	Parser(*this, content);
}

void Value::stringify(std::string& content) const noexcept
{
	if (this->type_ == kType::String && *this->string_ != content)
	{
		content = *string_;
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
	return *this->string_;
}

void Value::set_string(const std::string& string) noexcept
{
	if (this->type_ == kType::String)
	{
		delete this->string_;
		this->string_ = new std::string(string);
	}
	else
	{
		free();// free union space
		this->type_ = kType::String;
		this->string_ = new std::string(string);
	}
}

void Value::init(const Value& value) noexcept // move? or copy
{
	this->type_ = value.type_;
	this->number_ = 0;
	switch (this->type_)
	{
	case Number:
		this->number_ = value.number_;
		break;
	case String://copy on write
		this->string_ = new std::string(*value.string_);
		break;
	case Array:
		this->array_ = new std::vector<Value>(*value.array_);
		break;
	case Object:
		this->object_ = new std::unordered_map<std::string, Value>(*value.object_);
		break;
	default:
		break;
	}
}

void Value::free() noexcept
{
	//used to free the space of union
	switch (this->type_)
	{
	case String:
		delete this->string_;
		break;
	case Array:
		delete this->array_;
		break;
	case Object:
		delete this->object_;
		break;
	default:
		break;
	}
}
