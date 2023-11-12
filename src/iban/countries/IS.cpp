/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 *
 * Original code: https://github.com/pear/Validate_IS/blob/master/Validate/FI.php
 */

#include "iban/countries/IS.h"
#include "iban/bic.h"

#include <map>
#include <regex>
#include <vector>

using std::map;
using std::min;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::smatch;
using std::string;
using std::vector;

namespace iban
{
namespace countries
{

BBan_handler_IS::BBan_handler_IS(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_IS::is_valid_checksum(std::string const& bban) const
{
    static const int multiplier[6] = {2, 3, 4, 5, 6, 7};
    int sum = 0;

    auto it = bban.rbegin() + 2;
    for (auto i = 0; i < 8; ++i, ++it)
    {
        sum += (*it - '0') * multiplier[i % 6];
    }

    auto check = (11 - sum % 11) % 11;

    return check == *(bban.rbegin() + 1) - '0';
}

bool BBan_handler_IS::is_valid_bankcode(std::string const& bban) const
{
    // Though schwifty ha a manually created bank list, the codes there are
    // only 2 chars long and seemingly incomplete. Hence no validation for
    // that.

    return true;
}

__attribute__((weak)) bool BBan_handler_IS::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_IS::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    const regex numeric("^.*([0-9]{22}).*$");
    const regex formatted("^[^0-9]*([0-9]{4})-([0-9]{2})-([0-9]{1,6})-([0-9]{6})-([0-9]{4}).*$");
    auto trimmed_bban = regex_replace(bban, trim, "");

    // if formatted, then most probably it is in local format
    smatch formatted_result;
    regex_match(trimmed_bban, formatted_result, formatted);

    if (formatted_result.size() == 6)
    {
        auto bank = formatted_result[1].str();
        auto type = formatted_result[2].str();
        auto account = formatted_result[3].str();
        auto id_1 = formatted_result[4].str();
        auto id_2 = formatted_result[5].str();

        if (account.size() < 6)
        {
            account = string(6 - account.size(), '0') + account;
        }

        return bank + type + account + id_1 + id_2;
    }

    // if full numeric, than already prepared for IBAN creation
    smatch numeric_result;
    regex_match(trimmed_bban, numeric_result, numeric);

    if (numeric_result.size() == 2)
    {
        return numeric_result[1].str();
    }

    return "";
}

std::string BBan_handler_IS::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 4);
    auto type = bban.substr(4, 2);
    auto account = bban.substr(6, 6);
    auto id_1 = bban.substr(12, 6);
    auto id_2 = bban.substr(18, 4);

    return bank + "-" + type + "-" + account + "-" + id_1 + "-" + id_2;
}

} // namespace countries
} // namespace iban
