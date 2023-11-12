/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/countries/MT.h"
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

BBan_handler_MT::BBan_handler_MT(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_MT::is_valid_checksum(std::string const& bban) const
{
    // No algorithm known
    return true;
}

__attribute__((weak)) bool BBan_handler_MT::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_MT::preformat(std::string const& bban) const
{
    auto temp_bban = regex_replace(bban, regex("[^0-9A-Z]"), "");

    return temp_bban;
}

std::string BBan_handler_MT::format(std::string const& bban) const
{
    // no formal standard exists
    // seen places where BIC and account number (without leading zeros) were present
    // to not lose any info, we print in 3 pieces

    auto bank = bban.substr(0, 4);
    auto sort = bban.substr(4, 5); // in place of the branch identifier
    auto account = bban.substr(9);

    return bank + " " + sort + " " + account;
}


} // namespace countries
} // namespace iban
