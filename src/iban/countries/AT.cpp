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

#include "iban/bic.h"
#include "iban/countries/AT.h"

#include <regex>

using std::regex;
using std::regex_match;
using std::regex_replace;
using std::smatch;
using std::string;

namespace iban
{
namespace countries
{

BBan_handler_AT::BBan_handler_AT(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_AT::is_valid_checksum(std::string const& bban) const
{
    // algorithm (if any) unknown
    // TODO: look for APSS (Austrian Payment Services System)
    return true;
}

__attribute__((weak)) bool BBan_handler_AT::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_AT::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    const regex numeric("^.*([0-9]{16}).*$");
    const regex formatted("^[^0-9]*([0-9]{5})-([0-9]{1,11})[^0-9]*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    // if formatted, then most probably it is in local format
    smatch formatted_result;
    regex_match(trimmed_bban, formatted_result, formatted);

    if (formatted_result.size() == 3)
    {
        auto bank = formatted_result[1].str();
        auto account = formatted_result[2].str();

        account = string(11 - account.size(), '0') + account;

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

std::string BBan_handler_AT::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_AT::format(std::string const& bban) const
{
    if (bban.size() != m_iban_structure.bban_length)
    {
        return bban;
    }

    return bban.substr(0, 5) + "-" + bban.substr(5);
}

} // namespace countries
} // namespace iban
