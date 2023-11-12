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

#include "iban/countries/NO.h"
#include "iban/bic.h"
#include "iban/common.h"

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

BBan_handler_NO::BBan_handler_NO(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_NO::is_valid_checksum(std::string const& bban) const
{
    if (bban[4] == '0' && bban[5] == '0')
    {
        return true;
    }

    static const vector<int> weights = {5, 4, 3, 2, 7, 6, 5, 4, 3, 2, 1};

    int sum = 0;
    for (size_t i = 0; i < bban.size(); ++i)
    {
        sum += (bban[i] - '0') * weights[i];
    }

    return (sum % 11 == 0);
}

__attribute__((weak)) bool BBan_handler_NO::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_NO::preformat(std::string const& bban) const
{
    // remove all unwanted chars
    auto temp_bban = regex_replace(bban, regex("[^0-9]"), "");

    return temp_bban;
}

std::string BBan_handler_NO::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_NO::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 4);   // Bank
    auto branch = bban.substr(4, 2); //
    auto account = bban.substr(6);   // Account

    return bank + "." + branch + "." + account;
}

} // namespace countries
} // namespace iban
