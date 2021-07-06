#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "JsonException.h"

enum class kType
{
	Null, True, False, Number, String, Array, Object
};

class Value
{
public:
	void parse(const std::string& content); // parse the string of the content and use the Value contain the result;
	void stringify(std::string& content) const noexcept; // convert Value to string

	//tpye
	kType get_type() const noexcept;
	void set_type(kType type) noexcept;

	//number
	double get_number() const noexcept;
	void set_number(double number) noexcept;

	//string
	const std::string& get_string() const noexcept; //right ref? or const ref£¿
	void set_string(const std::string& string) noexcept;

	//array
	int get_array_size() const noexcept;
	const Value& get_array_element(size_t index) const noexcept;//
	void set_array(const std::vector<Value>& array) noexcept; // move? copy? or both?
	void array_push_back(const Value& value) noexcept;
	void array_pop_back() noexcept;
	void array_insert(const Value& value, size_t index) noexcept;
	void array_erase(size_t index, size_t count) noexcept; //count? 
	void array_clear() noexcept;

	//object
	const std::string& get_object_key(const Value& value) const noexcept;
	size_t get_object_key_length(const Value& value) const noexcept;
	const Value& get_object_value(const std::string& key) const noexcept;
	void set_object_value(const std::string& key, const Value& value) noexcept;
	void set_object(const std::unordered_map<std::string, Value>& object) noexcept;
	void object_remove(std::string& key) noexcept;
	void object_clear() noexcept;

	//overload
	Value& operator=(const Value& value) noexcept;
	bool operator!=(const Value& value) noexcept;
	bool operator==(const Value& value) noexcept;

	//construct and deconstruct
	Value() noexcept { type_ = kType::Null; };
	Value(const Value& value) noexcept { init(value); }
	~Value() noexcept;

private:
	void init(const Value& value) noexcept;
	void free() noexcept;

	kType type_ = kType::Null;
	union 
	{
		double number_;

		std::string* string_;
		std::vector<Value>* array_;
		std::unordered_map<std::string, Value>* object_;
	};

};

