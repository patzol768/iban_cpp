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

#include "iban/countries/TR.h"
#include "iban/bic.h"

#include <regex>

using std::min;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::smatch;
using std::string;

namespace iban
{
namespace countries
{

BBan_handler_TR::BBan_handler_TR(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_TR::is_valid_checksum(std::string const& bban) const
{
    // algorithms are bank specific and definitions not found online (at least not yet)
    return true;
}

__attribute__((weak)) bool BBan_handler_TR::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_TR::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    // const regex formatted("^[^0-9]*([0-9]{4})([0-9]{5})([0-9A-Z]{1,19}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    // local account numbers are 19 chars long at most
    // in IBAN, they are using 16 chars only for the account part
    // conversion alogirthm is not described in TR201.pdf
    //
    // so the local account to iban conversion is not handled


    if (trimmed_bban.size() == 16)
    {
        // this could be a preformatted BBAN part, let's return
        return trimmed_bban;
    }

    /*
        // trimming removes the separator spaces
        smatch formatted_result;
        regex_match(trimmed_bban, formatted_result, formatted);

        if (formatted_result.size() == 4)
        {
            auto bank = formatted_result[1].str();
            auto branch = formatted_result[2].str();
            auto account = formatted_result[3].str();

            branch = string(19 - branch.size(), '0') + branch;

            return bank + branch + account;
        }
    */
    return "";
}

std::string BBan_handler_TR::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 4);
    auto branch = bban.substr(4, 9);
    auto account = bban.substr(13);

    return bank + " " + branch + " " + account;
}

} // namespace countries
} // namespace iban
