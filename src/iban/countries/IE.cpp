/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 *
 * Original code: https://github.com/pear/Validate_IE/blob/master/Validate/FI.php
 */

#include "iban/countries/IE.h"
#include "iban/bic.h"

#include <regex>
#include <vector>

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

BBan_handler_IE::BBan_handler_IE(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_IE::is_valid_checksum(std::string const& bban) const
{
    // no common national algorithm
    // bank level algorithms not known

    return true;
}

__attribute__((weak)) bool BBan_handler_IE::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_IE::preformat(std::string const& bban) const
{
    // no "sort code" to "bank code" list is found so far,
    // hence we expect the "bank code" before the local account number format
    //
    // "bank code" seem to be the first 4 chars from the BIC

    const regex trim("\\s");
    const regex alphanumeric("^.*([0-9A-Z]{4}[0-9]{14}).*$");
    const regex formatted("^[^0-9A-Z]*([0-9A-Z]{4})([0-9]{2})[-]?([0-9]{2})[-]?([0-9]{2})([0-9]{1,8}).*$");
    auto trimmed_bban = regex_replace(bban, trim, "");

    // if formatted, then most probably it is in local format
    smatch formatted_result;
    regex_match(trimmed_bban, formatted_result, formatted);

    if (formatted_result.size() == 6)
    {
        auto bank = formatted_result[1].str();
        auto sort_1 = formatted_result[2].str(); // sort code is put into the "branch" in iban
        auto sort_2 = formatted_result[3].str();
        auto sort_3 = formatted_result[4].str();
        auto account = formatted_result[5].str();

        if (account.size() < 8)
        {
            account = string(8 - account.size(), '0') + account;
        }

        return bank + sort_1 + sort_2 + sort_3 + account;
    }

    // if full alphanumeric, than already prepared for IBAN creation
    smatch numeric_result;
    regex_match(trimmed_bban, numeric_result, alphanumeric);

    if (numeric_result.size() == 2)
    {
        return numeric_result[1].str();
    }

    return "";
}

std::string BBan_handler_IE::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 4);
    auto sort_1 = bban.substr(4, 2);
    auto sort_2 = bban.substr(6, 2);
    auto sort_3 = bban.substr(8, 2);
    auto account = bban.substr(10);

    return bank + " " + sort_1 + "-" + sort_2 + "-" + sort_3 + "-" + account;
}

} // namespace countries
} // namespace iban
