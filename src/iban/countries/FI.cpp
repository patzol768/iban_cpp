/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 *
 * Original code: https://github.com/pear/Validate_FI/blob/master/Validate/FI.php
 */

#include "iban/countries/FI.h"
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

BBan_handler_FI::BBan_handler_FI(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_FI::is_valid_checksum(std::string const& bban) const
{
    static const int multiplier[2] = {2, 1};
    int sum = 0;

    auto it = bban.rbegin() + 1;
    for (auto i = 0; i < 13; ++i, ++it)
    {
        auto value = (*it - '0') * multiplier[i % 2];
        sum += value % 10;
        if (value > 9)
        {
            sum++;
        }
    }

    auto check = (10 - sum % 10) % 10;

    return check == bban.back() - '0';
}

__attribute__((weak)) bool BBan_handler_FI::is_valid_ext(std::string const& bban) const
{
    return true;
}

// Group 1: First digit is 1, 2, 3, 6 or 8
// $bankGroup1 = array('1', '2', '3', '6', '8');
// Group 2: First digit is 4 or 5
// $bankGroup2 = array('4', '5');
// if (in_array($bankType, $bankGroup1)) {
//     // Group 1: 999999-99999 -> 999999-00099999
//     $number = $regs[1] . str_pad($regs[2], 8, 0, STR_PAD_LEFT);
//     // Group 2: 999999-99999 -> 999999-90009999
// } else if (in_array($bankType, $bankGroup2)) {
//     $number = $regs[1] . substr($regs[2], 0, 1) .
//         str_pad(substr($regs[2], 1, 7), 7, 0, STR_PAD_LEFT);
// }

std::string BBan_handler_FI::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    const regex numeric("^.*([0-9]{14}).*$");
    const regex formatted("^[^0-9]*([0-9]{6})-([0-9]{2,8}).*$");
    const map<char, int> groups = {{'1', 1}, {'2', 1}, {'3', 1}, {'4', 2}, {'5', 2}, {'6', 1}, {'8', 1}};
    auto trimmed_bban = regex_replace(bban, trim, "");

    auto group_it = groups.find(trimmed_bban[0]);
    if (group_it == groups.end())
    {
        return "";
    }

    // if formatted, then most probably it is in local format
    smatch formatted_result;
    regex_match(trimmed_bban, formatted_result, formatted);

    if (formatted_result.size() == 3)
    {
        auto bank = formatted_result[1].str();
        auto account = formatted_result[2].str();

        if (account.size() < 8)
        {
            switch (group_it->second)
            {
                case 1:
                    account = string(8 - account.size(), '0') + account;
                    break;

                case 2:
                    account = account.substr(0, 1) + string(8 - account.size(), '0') + account.substr(1);
                    /* code */
                    break;

                default:
                    return "";
            }
        }

        return bank + account;
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

std::string BBan_handler_FI::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 6); // Bank
    auto account = bban.substr(6); // Account

    return bank + "-" + account;
}

} // namespace countries
} // namespace iban
