#pragma once

#include <stdexcept>

namespace iban
{

class IbanError : public std::logic_error
{
    public:
    IbanError(std::string error)
    : std::logic_error(error) {};
};

} // namespace iban