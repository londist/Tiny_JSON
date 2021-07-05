#pragma once

#include <string>
#include <stdexcept>

class JsonException final : public std::logic_error 
{

public:
    JsonException(const std::string& errMsg) : logic_error(errMsg) { }
};
