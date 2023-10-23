#pragma once

#include <stdexcept>

namespace iban
{

class Iban_error : public std::logic_error
{
    public:
    Iban_error(std::string error)
    : std::logic_error(error) {};
};

} // namespace iban