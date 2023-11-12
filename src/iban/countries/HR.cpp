/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 *
 * Original code: https://github.com/globalcitizen/php-iban/blob/master/php-iban.php
 *
 */

#include "iban/countries/HR.h"
#include "iban/bic.h"
#include "iban/common.h"
#include "iban/iban.h"

#include <regex>
#include <vector>

using std::min;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::smatch;
using std::string;
using std::to_string;
using std::vector;

namespace iban
{
namespace countries
{

BBan_handler_HR::BBan_handler_HR(std::string const& country)
: BBan_handler(country)
{
}

int BBan_handler_HR::checksum_iso7064_mod11_10(std::string const& s) const
{
    // Croation account identifiers contain only numbers
    // hence this algorithm is for numbers only

    size_t p = 0;
    int part = 10;

    do
    {
        part += s[p] - '0';
        part %= 10;
        part = (part != 0) ? part * 2 : 20;
        part %= 11;
    } while (++p < s.size());

    return 11 - part;
}

bool BBan_handler_HR::is_valid_checksum(std::string const& bban) const
{
    // not checking the bankcode's checksum, since all codes are coming from a list

    auto checksum = checksum_iso7064_mod11_10(bban.substr(7, 9)) % 10;

    return (checksum == bban[16] - '0');
}

__attribute__((weak)) bool BBan_handler_HR::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_HR::preformat(std::string const& bban) const
{
    // remove any separators and not allowed chars
    auto temp_bban = regex_replace(bban, regex("[^0-9]"), "");

    return temp_bban;
}

std::string BBan_handler_HR::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_HR::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 7); // Bank
    auto account = bban.substr(7); // Account

    return bank + "-" + account;
}

} // namespace countries
} // namespace iban
