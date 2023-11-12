/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 * 
 * Original code: https://github.com/mdomke/schwifty/schwifty/checksum/france.py
 */ 

#include "iban/countries/FR.h"
#include "iban/bic.h"
#include "iban/common.h"

#include <regex>

using std::min;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::smatch;
using std::string;
using std::to_string;

namespace iban
{
namespace countries
{

BBan_handler_FR::BBan_handler_FR(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_FR::is_valid_checksum(std::string const& bban) const
{
    // algorithm: https://www.dcode.fr/bban-check

    if (bban.size() != 23)
    {
        return false;
    }

    auto numeric_bban = to_numeric(bban);

    if (numeric_bban.empty())
    {
        return false;
    }

    return (bignum_mod97(numeric_bban) == 0);
}

__attribute__((weak)) bool BBan_handler_FR::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_FR::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    // space is used as separator, but trimming removes
    const regex alphanumeric("^.*([0-9]{5}[0-9]{5}[0-9A-Z]{11}[0-9]{2}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    smatch alnum_result;
    regex_match(trimmed_bban, alnum_result, alphanumeric);

    if (alnum_result.size() == 2)
    {
        return alnum_result[1].str();
    }

    return "";
}

std::string BBan_handler_FR::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_FR::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 5);      // "Code banque"
    auto branch = bban.substr(5, 5);    // "Code guichet"
    auto account = bban.substr(10, 11); // "Numéro de compte"
    auto checksum = bban.substr(21, 2);

    return bank + " " + branch + " " + account + " " + checksum;
}

std::string BBan_handler_FR::to_numeric(std::string const& bban) const
{
    // 'A'=1, 'B'=2, etc.
    const int32_t numbers[26] = {'1', '2', '3', '4', '5', '6', '7', '8', '9',
                                 '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                 '2', '3', '4', '5', '6', '7', '8', '9'};

    string result;
    for (auto const& c : bban)
    {
        if (c >= '0' && c <= '9')
        {
            result += c;
        }
        else if (c >= 'A' && c <= 'Z')
        {
            result += numbers[c - 'A'];
        }
        else
        {
            return "";
        }
    }

    return result;
}

int BBan_handler_FR::bignum_mod97(std::string const& number) const
{
    size_t p = 0;
    size_t digits = 16; // signed 64 bit integer has 19 digits, so to avoid overflow use 16 + 2 (remainder)
    int64_t remainder = 0;

    while (p < number.size())
    {
        string s_segment = to_string(remainder) + number.substr(p, digits);
        int64_t i_segment = stoll(s_segment);
        remainder = i_segment % 97LL;

        p += digits;
    }

    return (int)remainder;
}

} // namespace countries
} // namespace iban
