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

#include "iban/countries/MK.h"
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

BBan_handler_MK::BBan_handler_MK(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_MK::is_valid_checksum(std::string const& bban) const
{
    auto checksum = Iban::checksum_mod97(bban);

    return (checksum == "97");
}

bool BBan_handler_MK::is_valid_bankcode(std::string const& bban) const
{
    return true;
}

__attribute__((weak)) bool BBan_handler_MK::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_MK::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    // space is used as separator, but trimming removes
    const regex alphanumeric("^.*([0-9]{3}[0-9A-Z]{10}[0-9]{2}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    smatch alnum_result;
    regex_match(trimmed_bban, alnum_result, alphanumeric);

    if (alnum_result.size() == 2)
    {
        return alnum_result[1].str();
    }

    return "";
}

std::string BBan_handler_MK::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_MK::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 3);     // Bank
    auto account = bban.substr(3, 10); // Account
    auto check = bban.substr(13);      // Checksum

    return bank + " " + account + " " + check;
}

} // namespace countries
} // namespace iban
