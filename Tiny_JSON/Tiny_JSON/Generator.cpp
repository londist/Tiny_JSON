#include "Generator.h"

Generator::Generator(const Value& value, std::string& json_string)
	:json_(json_)
{
	json_.clear();
	stringify_value(value);
}

void Generator::stringify_value(const Value& value) 
{
	switch (value.get_type())
	{
	case kType::Null:
		this->json_ += "null";
		break;
	case kType::True:
		this->json_ += "true";
		break;
	case kType::False:
		this->json_ += "false";
		break;
	case kType::Number:
		this->json_ += std::to_string(value.get_number());
		break;
	case kType::String:
		stringify_string(value.get_string());
		break;
	case kType::Array:
		json_ += '[';
		for (int i = 0; i < value.get_array_size(); i++)
		{
			if (i > 0)
				json_ += ',';
			this->stringify_value(value.get_array_element(i));
		}
		json_ += ']';
		break;
	case kType::Object:
		json_ += '{';
		int count = 0;
		for (auto& member : value.get_object())
		{
			if (count > 0) this->json_ += ',';
			this->stringify_string(member.first);
			this->json_ += ':';
			this->stringify_value(member.second);
			count++;
		}
		json_ += '}';
		break;
	}
}

void Generator::stringify_string(const std::string string)
{
	this->json_ += '\"';
	
	for (const auto& ch : string)
	{
		switch (ch)
		{
		case '\\':
			this->json_ += "\\\\";
			break;
		case '\t':
			this->json_ += "\\t";
			break;
		case '\"':
			this->json_ += "\\\"";
			break;
		case '\b':
			this->json_ += "\\b";
			break;
		case '\f':
			this->json_ += "\\f";
			break;
		case '\n':
			this->json_ += "\\n";
			break;
		case '\r':
			this->json_ += "\\r";
			break;
		default:
			if (ch < 0x20)
			{
				char buffer[7] = { 0 };
				sprintf(buffer, "\\u%04X", ch);
				json_ += buffer;
			}
			else
			{
				json_ += ch;
			}
			break;
		}
	}
	json_ += '\"';
}


