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

#include "iban/countries/SI.h"
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

BBan_handler_SI::BBan_handler_SI(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_SI::is_valid_checksum(std::string const& bban) const
{
    auto checksum = Iban::checksum_mod97(bban);

    return (checksum == "97");
}

bool BBan_handler_SI::is_valid_bankcode(std::string const& bban) const
{
    return true;
}

__attribute__((weak)) bool BBan_handler_SI::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_SI::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    const regex alphanumeric("^.*([0-9]{15}).*$");
    const regex formatted("^[^0-9]*([0-9]{5})-([0-9]{8})([0-9]{2}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    // if formatted, then most probably it is in local format
    smatch formatted_result;
    regex_match(trimmed_bban, formatted_result, formatted);

    if (formatted_result.size() == 4)
    {
        auto bank = formatted_result[1].str();
        auto account = formatted_result[2].str();
        auto check = formatted_result[3].str();

        return bank + account + check;
    }

    // if full alphanumeric, than already prepared for IBAN creation
    smatch alnum_result;
    regex_match(trimmed_bban, alnum_result, alphanumeric);

    if (alnum_result.size() == 2)
    {
        return alnum_result[1].str();
    }

    return "";
}

std::string BBan_handler_SI::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_SI::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 5); // Bank
    auto account = bban.substr(5); // Account

    return bank + "-" + account;
}

} // namespace countries
} // namespace iban
