/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 *
 * Original code: https://github.com/globalcitizen/php-iban/blob/master/php-iban.php
 */

#include "iban/countries/NL.h"
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

BBan_handler_NL::BBan_handler_NL(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_NL::is_valid_checksum(std::string const& bban) const
{
    auto bank = bban.substr(0, 4);
    if (bank == "INGB")
    {
        return true;
    }

    int checksum = 0;
    for (int i = 0; i < 10; i++)
    {
        checksum += (bban[i + 4] - '0') * (10 - i);
    }

    return ((checksum % 11) == 0);
}

__attribute__((weak)) bool BBan_handler_NL::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_NL::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    // space is used as separator, but trimming removes
    const regex alphanumeric("^.*([A-Z]{4}[0-9]{10}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    smatch alnum_result;
    regex_match(trimmed_bban, alnum_result, alphanumeric);

    if (alnum_result.size() == 2)
    {
        return alnum_result[1].str();
    }

    return "";
}

std::string BBan_handler_NL::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_NL::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 4);      // Bank
    auto branch = bban.substr(4, 2);    // Branch
    auto account_1 = bban.substr(6, 2); // Account parts
    auto account_2 = bban.substr(8);

    return bank + " " + branch + " " + account_1 + " " + account_2;
}

} // namespace countries
} // namespace iban
