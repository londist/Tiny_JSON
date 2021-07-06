#pragma once

#include "Value.h"
#include <cassert>

class Parser final
{
public:
	Parser(Value& value, const std::string& content);
private:
	void parse_whitespace() noexcept;
    void parse_value();
    void parse_literal(const char* literal, kType t);
    void parse_number();
    void parse_string();
    void parse_string_raw(std::string& string);
    void parse_hex4(const char*& p, unsigned& u);
    void parse_encode_utf8(std::string& s, unsigned u) const noexcept;
    void parse_array();
    void parse_object();

    Value& value_;
    const char* current_; 
};

inline void expect(const char*& c, char ch)
{
    assert(*c == ch);
    ++c;
}

