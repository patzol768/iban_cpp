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
 * BBBBRRRRCCAAAAAAAAAA
 *
 *  - B: bank
 *  - R: branch
 *  - C: checksum
 *       1st digit: bank + branch check
 *       2nd digit: account check
 *  - A: account
 */

#include "iban/countries/ES.h"
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

BBan_handler_ES::BBan_handler_ES(std::string const& country)
: BBan_handler(country)
{
}

int BBan_handler_ES::mod11(std::string const& account) const
{
    static const vector<int> weights = {1, 2, 4, 8, 5, 10, 9, 7, 3, 6};

    int sum = 0;
    for (size_t i = 0; i < account.size(); ++i)
    {
        sum += account[i] * weights[i];
    }

    auto checksum = (11 - (sum % 11)) % 11;
    return (checksum != 10) ? checksum : 1;
}

bool BBan_handler_ES::is_valid_checksum(std::string const& bban) const
{
    auto check_1 = mod11("00" + bban.substr(0, 8));
    auto check_2 = mod11(bban.substr(10));

    return (check_1 == bban[8] - '0' && check_2 == bban[9] - '0');
}

__attribute__((weak)) bool BBan_handler_ES::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_ES::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    // space is used as separator, but trimming removes
    const regex alphanumeric("^.*([0-9]{20}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    smatch alnum_result;
    regex_match(trimmed_bban, alnum_result, alphanumeric);

    if (alnum_result.size() == 2)
    {
        return alnum_result[1].str();
    }

    return "";
}

std::string BBan_handler_ES::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_ES::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 4);      // Bank
    auto branch = bban.substr(4, 2);    // Branch
    auto account_1 = bban.substr(6, 2); // Account parts
    auto account_2 = bban.substr(8);

    return bank + " " + branch + " " + account_1 + " " + account_2;
}

} // namespace countries
} // namespace iban
