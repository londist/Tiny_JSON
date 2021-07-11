#include "Value.h"
#include "Parser.h"
#include "Generator.h"
#include <cassert>

void Value::parse(const std::string& content)
{
	Parser(*this, content);
}

void Value::stringify(std::string& content) const noexcept
{
		Generator(*this, content);
}

kType Value::get_type() const noexcept
{
	return this->type_;
}

void Value::set_type(kType type) noexcept
{
	this->free();
	this->type_ = type;
}

double Value::get_number() const noexcept
{
	assert(this->type_ == kType::Number);
	return this->number_;
}

void Value::set_number(double number) noexcept
{
	this->free();
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
		this->free();// free union space
		this->type_ = kType::String;
		this->string_ = new std::string(string);
	}
}

int Value::get_array_size() const noexcept
{
	assert(this->type_ == kType::Array);
	return this->array_->size();
}

const Value& Value::get_array_element(size_t index) const noexcept
{
	assert(this->type_ == kType::Array);
	return this->array_->at(index);
}

void Value::set_array(const std::vector<Value>& array) noexcept
{
	if (this->type_ == kType::Array)
	{
		this->array_ = new std::vector<Value>(array);
	}
	else
	{
		this->free();
		this->type_ = kType::Array;
		this->array_ = new std::vector<Value>(array);
	}
}

void Value::array_push_back(const Value& value) noexcept
{
	assert(value.type_ == kType::Array);
	if (this->array_ != nullptr)
	{
		this->array_->push_back(value);
	}
}

void Value::array_pop_back() noexcept
{
	assert(this->type_ == kType::Array);
	if (this->array_ != nullptr)
	{
		this->array_->pop_back();
	}
}

void Value::array_insert(const Value& value, size_t index) noexcept
{
	assert(this->type_ == kType::Array);
	this->array_->insert(this->array_->begin() + index, value);
}

void Value::array_erase(size_t index, size_t count) noexcept
{
	assert(this->type_ == kType::Array);
	this->array_->erase(this->array_->begin() + index, this->array_->begin() + index + count);
}

void Value::array_clear() noexcept
{
	assert(this->type_ == kType::Array);
	this->array_->clear();
}
std::unordered_map<std::string, Value>& Value::get_object() const
{
	return *this->object_;
}

const std::string& Value::get_object_key(const Value& value) const //
{
	assert(this->type_ == kType::Object);
	for (auto it = this->object_->begin(); it != this->object_->end(); it++)
	{
		if (it->second == value)
			return it->first;
	}
	throw(JsonException("the value is not a element of object"));
}

size_t Value::get_object_key_length(const Value& value) const
{
	assert(this->type_ == kType::Object);
	return this->get_object_key(value).size();
}

const Value& Value::get_object_value(const std::string& key) const noexcept
{
	assert(this->type_ == kType::Object);
	return this->object_->find(key)->second;
}

void Value::set_object_value(const std::string& key, const Value& value) noexcept
{
	if (type_ == kType::Object)
	{
		this->object_->at(key) = value;
	}
	else
	{
		this->free();
		this->type_ = kType::Object;
		this->object_ = new std::unordered_map<std::string, Value>();
		this->object_->at(key) = value;
	}
}

void Value::set_object(const std::unordered_map<std::string, Value>& object) noexcept
{
	this->free();
	if (type_ == kType::Object)
	{
		this->object_ = new std::unordered_map<std::string, Value>(object);
	}
	else
	{
		this->type_ = kType::Object;
		this->object_ = new std::unordered_map<std::string, Value>(object);
	}
}

void Value::object_remove(std::string& key) noexcept
{
	assert(this->type_ == kType::Object);
	this->object_->erase(key);
}

void Value::object_clear() noexcept
{
	assert(this->type_ == kType::Object);
	this->object_->clear();
}

Value& Value::operator=(const Value& value) noexcept
{
	this->free();
	this->init(value);
	return (*this);
}

bool Value::operator!=(const Value& value) noexcept
{
	return !((*this) == value);
}

bool Value::operator==(const Value& value) noexcept
{
	if (this->type_ != value.type_)
	{
		return false;
	}
	switch (value.type_)
	{
	case kType::Number:
		return this->number_ == value.number_; //double number? it is ok?
	case kType::String:
		return this->string_ == value.string_;
	case kType::Array:
		return this->array_ == value.array_;
	case kType::Object:
		return this->object_ == value.object_;
	default:
		break;
	}
	return true;
}

void Value::init(const Value& value) noexcept // move? or copy
{
	this->type_ = value.type_;
	this->number_ = 0;
	switch (this->type_)
	{
	case kType::Number:
		this->number_ = value.number_;
		break;
	case kType::String://copy on write
		this->string_ = new std::string(*value.string_);
		break;
	case kType::Array:
		this->array_ = new std::vector<Value>(*value.array_);
		break;
	case kType::Object:
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
	case kType::String:
		delete this->string_;
		break;
	case kType::Array:
		delete this->array_;
		break;
	case kType::Object:
		delete this->object_;
		break;
	default:
		break;
	}
}
