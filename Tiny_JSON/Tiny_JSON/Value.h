#pragma once

#include <string>
#include <map>
#include <vector>

enum kType
{
	Null, True, False, Number, String, Array, Object
};

class Value
{
public:
	void parse(const std::string& content);
	void stringify(std::string& content) const noexcept;



private:
	kType type_;
	union 
	{
		double num_;
		std::string str_;
		std::vector<Value> arr_;
		std::map<std::string, Value> obj_;
	};

};

