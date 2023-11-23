/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/countries/UK.h"
#include "iban/bic.h"

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

BBan_handler_UK::BBan_handler_UK(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_UK::is_valid_checksum(std::string const& bban) const
{
    // ...

    return true;
}

__attribute__((weak)) bool BBan_handler_UK::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_UK::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    const regex numeric("^.*([0-9]{14}).*$");
    const regex formatted("^[^0-9]*([0-9]{2})-([0-9]{2})-([0-9]{2})([0-9]{7,8}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    // if formatted, then most probably it is in local format
    smatch formatted_result;
    regex_match(trimmed_bban, formatted_result, formatted);

    if (formatted_result.size() == 5)
    {
        auto sort_code_1 = formatted_result[1].str();
        auto sort_code_2 = formatted_result[2].str();
        auto sort_code_3 = formatted_result[3].str();
        auto account = formatted_result[4].str();

        return sort_code_1 + sort_code_2 + sort_code_3 + account;
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

std::string BBan_handler_UK::format(std::string const& bban) const
{
    auto sort_1 = bban.substr(0, 2);
    auto sort_2 = bban.substr(2, 2);
    auto sort_3 = bban.substr(4, 2);
    auto account = bban.substr(6);

    return sort_1 + "-" + sort_2 + "-" + sort_3 + "-" + account;
}

} // namespace countries
} // namespace iban
