/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 *
 * Original code: https://github.com/pear/Validate_CH/blob/master/Validate/FI.php
 */

// Note: as there's no national account format and there were no links to the
//       definition of bank specific local account number to IBAN, it is a
//       non-trivial task to convert 

#include "iban/countries/CH.h"
#include "iban/bic.h"

#include <map>
#include <regex>
#include <vector>

using std::map;
using std::min;
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

BBan_handler_CH::BBan_handler_CH(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_CH::is_valid_checksum(std::string const& bban) const
{
    // some banks may use check digits, but algorithms are not known for me
    return true;
}

__attribute__((weak)) bool BBan_handler_CH::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_CH::preformat(std::string const& bban) const
{
    // this way we can't tell if the bank code itself had wrong length
    // so we can end up with a wrong IBAN at the end!
    //
    // TODO: check bank code first and only than remove non-numeric chars
    auto temp_bban = regex_replace(bban, regex("[^0-9A-Z]"), "");

    auto bank = temp_bban.substr(0, 5);
    auto account = temp_bban.substr(5);

    if (account.size() < 12)
    {
        account = string(12 - account.size(), '0') + account;
    }

    return bank + account;
}

std::string BBan_handler_CH::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 5);
    auto account = bban.substr(5);

    return bank + " " + account;
}

} // namespace countries
} // namespace iban
