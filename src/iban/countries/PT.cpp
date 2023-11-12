/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/countries/PT.h"
#include "iban/bic.h"

#include <regex>

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

BBan_handler_PT::BBan_handler_PT(std::string const& country)
: BBan_handler(country)
{
}

std::string BBan_handler_PT::checksum_iso7064_mod97_10(std::string const& s) const
{
    // account identifiers contain only numbers
    // hence this algorithm is for numbers only

    size_t p = 0;
    int part = 0;

    do
    {
        part += s[p] - '0';
        part *= 10;
        part %= 97;
    } while (++p < s.size());

    part *= 10;
    part %= 97;

    auto check = (98 - part) % 97;

    return (check > 9) ? to_string(check) : "0" + to_string(check);
}

bool BBan_handler_PT::is_valid_checksum(std::string const& bban) const
{
    auto account = bban.substr(0, 19);
    auto checksum = checksum_iso7064_mod97_10(account);

    return (checksum == bban.substr(19, 2));
}

__attribute__((weak)) bool BBan_handler_PT::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_PT::preformat(std::string const& bban) const
{
    auto temp_bban = regex_replace(bban, regex("[^0-9A-Z]"), "");

    return temp_bban;
}

std::string BBan_handler_PT::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 4);
    auto branch = bban.substr(4, 5);
    auto account = bban.substr(9, 11);
    auto check = bban.substr(20, 2);

    return bank + "." + branch + "." + account + "." + check;
}


} // namespace countries
} // namespace iban
