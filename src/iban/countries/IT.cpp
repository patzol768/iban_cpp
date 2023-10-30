/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */ 

#include "iban/countries/IT.h"
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

BBan_handler_IT::BBan_handler_IT(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_IT::is_valid_checksum(std::string const& bban) const
{
    const int odd_values[26] = {1, 0, 5, 7, 9, 13, 15, 17, 19, 21, 2, 4, 18, 20, 11, 3, 6, 8, 12, 14, 16, 10, 22, 25, 24, 23};

    auto checksum = bban[0];
    auto account_part = bban.substr(1);

    int sum = 0;
    for (size_t i = 0; i < account_part.size(); i++)
    {
        auto c = account_part[i];
        if (c >= '0' && c <= '9')
        {
            c -= '0';
        }
        else if (c >= 'A' && c <= 'Z')
        {
            c -= 'A';
        }
        else
        {
            return false;
        }

        sum += (i % 2) ? /*even*/ c : /*odd*/ odd_values[c];
    }

    auto calc_checksum = (sum % 26) + 'A';

    return (checksum == calc_checksum);
}

__attribute__((weak)) bool BBan_handler_IT::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_IT::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    // space is used as separator, but trimming removes
    const regex alphanumeric("^.*([A-Z]{1}[0-9]{5}[0-9]{5}[0-9A-Z]{12}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    smatch alnum_result;
    regex_match(trimmed_bban, alnum_result, alphanumeric);

    if (alnum_result.size() == 2)
    {
        return alnum_result[1].str();
    }

    return "";
}

std::string BBan_handler_IT::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_IT::format(std::string const& bban) const
{
    auto cin = bban.substr(0, 1);    // CIN (Check char)
    auto bank = bban.substr(1, 5);   // ABI (National bank code)
    auto branch = bban.substr(6, 5); // CAB (Branch code)
    auto account = bban.substr(11);  // Numero di conto corrente (Account number)

    return cin + " " + bank + " " + branch + " " + account;
}

} // namespace countries
} // namespace iban
