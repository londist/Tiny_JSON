#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "Value.h"

class Generator final
{
public:
	Generator(const Value& value, std::string& json);
private:
	void stringify_value(const Value &value);
	void stringify_string(const std::string string);

	std::string& json_;
};

