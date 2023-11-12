/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/countries/LU.h"
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

BBan_handler_LU::BBan_handler_LU(std::string const& country)
: BBan_handler(country)
{
}

BBan_type BBan_handler_LU::get_bban_type() const
{
    return BBan_type::IBAN;
}

bool BBan_handler_LU::is_valid_checksum(std::string const& bban) const
{
    // IBAN validation is being used, since IBAN is used instead of BBAN
    return true;
}

__attribute__((weak)) bool BBan_handler_LU::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_LU::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    const regex alphanumeric("^.*([A-Z]{2}[0-9]{2}[0-9A-Z]{3}[0-9A-Z]{13}).*$");
    auto trimmed_bban = regex_replace(bban, trim, "");

    // Luxemburg simply uses IBAN instead of BBAN
    smatch numeric_result;
    regex_match(trimmed_bban, numeric_result, alphanumeric);

    if (numeric_result.size() == 2)
    {
        return numeric_result[1].str();
    }

    return "";
}

std::string BBan_handler_LU::format(std::string const& bban) const
{
    auto formatted = bban.substr(0, 4);
    size_t p = 4;

    while (p < bban.size())
    {
        formatted += " " + bban.substr(p, 4);
        p += 4;
    }

    return formatted;
}

} // namespace countries
} // namespace iban
