/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/countries/EE.h"
#include "iban/bic.h"

#include <regex>

using std::min;
using std::regex;
using std::regex_replace;
using std::string;

namespace iban
{
namespace countries
{

BBan_handler_EE::BBan_handler_EE(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_EE::is_valid_checksum(std::string const& bban) const
{
    int sum = 0;
    static const int multiplier[3] = {7, 3, 1};

    int p = min(bban.find_first_not_of('0', 2), bban.size());

    if (bban[0] != bban[p] || bban[1] != bban[p + 1])
    {
        return false;
    }

    auto it = bban.rbegin() + 1;
    for (auto i = 0; i < bban.size() - p - 1; ++i, ++it)
    {
        sum += (*it - '0') * multiplier[i % 3];
    }

    auto check = (10 - sum % 10) % 10;

    return check == bban.back() - '0';
}

__attribute__((weak)) bool BBan_handler_EE::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_EE::preformat(std::string const& bban) const
{
    // remove any separators and not allowed chars
    auto temp_bban = regex_replace(bban, regex("[^0-9]"), "");

    // 16 - already in BBAN format
    // 15 - something strange
    if (temp_bban.size() > 14)
    {
        return bban;
    }

    // remove accidental leading zeros
    if (temp_bban[0] == '0')
    {
        temp_bban.erase(0, min(temp_bban.find_first_not_of('0'), temp_bban.size()));
    }

    auto bank = temp_bban.substr(0, 2);

    if (temp_bban.size() < 14)
    {
        temp_bban = bank + string(14 - temp_bban.size(), '0') + temp_bban;
    }
    else
    {
        temp_bban = bank + temp_bban;
    }

    return temp_bban;
}

std::string BBan_handler_EE::format(std::string const& bban) const
{
    auto short_account = bban;
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));

    return bban;
}

} // namespace countries
} // namespace iban
