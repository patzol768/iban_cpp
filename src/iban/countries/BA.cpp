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

#include "iban/countries/BA.h"
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

BBan_handler_BA::BBan_handler_BA(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_BA::is_valid_checksum(std::string const& bban) const
{
    auto checksum = Iban::checksum_mod97(bban);

    return (checksum == "97");
}

bool BBan_handler_BA::is_valid_bankcode(std::string const& bban) const
{
    return true;
}

__attribute__((weak)) bool BBan_handler_BA::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_BA::preformat(std::string const& bban) const
{
    // remove any separators and not allowed chars
    auto temp_bban = regex_replace(bban, regex("[^0-9]"), "");

    return temp_bban;
}

std::string BBan_handler_BA::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_BA::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 3);    // Bank
    auto branch = bban.substr(3, 3);  // Branch
    auto account = bban.substr(6, 8); // Account
    auto check = bban.substr(14);     // Checksum

    return bank + "-" + branch + "-" + account + "-" + check;
}

} // namespace countries
} // namespace iban
