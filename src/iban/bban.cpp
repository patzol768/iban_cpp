/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/bban.h"
#include "iban/bic.h"

#include <algorithm>

using std::string;
using std::transform;
using std::unique_ptr;

namespace iban
{

BBan_handler::BBan_handler(std::string const& country)
: m_country(country)
, m_iban_structure(Iban_structure_repository::get_instance()->get_by_country(country))
{
}

std::string const& BBan_handler::get_country() const
{
    return m_country;
}

BBan_type BBan_handler::get_bban_type() const
{
    return BBan_type::NATIONAL;
}

bool BBan_handler::is_valid(std::string const& bban) const
{
    return is_valid_length(bban) && is_valid_checksum(bban) && is_valid_bankcode(bban) && is_valid_ext(bban);
}

bool BBan_handler::is_valid_length(std::string const& bban) const
{
    // the iban structure defines the positions from after the iban checksum
    // when the bban is also an iban, we must consider these extra chars
    auto shift = get_bban_shift(m_country);

    return (bban.size() == m_iban_structure.bban_length + shift);
}

bool BBan_handler::is_valid_checksum(std::string const& bban) const
{
    return false;
}

bool BBan_handler::is_valid_bankcode(std::string const& bban) const
{
    if (!Bic_repository::get_instance()->has_bank_list(m_country))
    {
        // cannot validate, no bank list for the given country
        // (not the best, but can't really do else)
        return true;
    }

    // the iban structure defines the positions from after the iban checksum
    // when the bban is also an iban, we must align the positions
    auto shift = get_bban_shift(m_country);

    // validates only bank code
    // if country has branch validation, the specific subclass must override this one
    auto bankcode = bban.substr(m_iban_structure.bank_code.first + shift, m_iban_structure.bank_code.second - m_iban_structure.bank_code.first);

    auto bank = Bic_repository::get_instance()->get_by_country_bankcode(m_country, bankcode);

    return !bank.empty();
}

bool BBan_handler::is_valid_ext(std::string const& bban) const
{
    return false;
}

std::string BBan_handler::preformat(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler::format(std::string const& bban) const
{
    return bban;
}

#define CALL(fn)                                                                  \
    auto handler = BBan_handler_factory::get_instance()->get_by_country(country); \
    if (!handler)                                                                 \
    {                                                                             \
        return false;                                                             \
    }                                                                             \
    return handler->is_##fn(bban);

BBan_type BBan_handler::get_bban_type(std::string const& country)
{
    auto handler = BBan_handler_factory::get_instance()->get_by_country(country);
    if (!handler)
    {
        return BBan_type::NATIONAL;
    }
    return handler->get_bban_type();
}

bool BBan_handler::is_valid(std::string const& country, std::string const& bban)
{
    CALL(valid);
}

bool BBan_handler::is_valid_length(std::string const& country, std::string const& bban)
{
    CALL(valid_length);
}

bool BBan_handler::is_valid_checksum(std::string const& country, std::string const& bban)
{
    CALL(valid_checksum);
}

bool BBan_handler::is_valid_bankcode(std::string const& country, std::string const& bban)
{
    CALL(valid_bankcode);
}

bool BBan_handler::is_valid_ext(std::string const& country, std::string const& bban)
{
    CALL(valid_ext);
}

std::string BBan_handler::preformat(std::string const& country, std::string const& bban)
{
    auto bban_handler = BBan_handler_factory::get_instance()->get_by_country(country);
    if (!bban_handler)
    {
        return bban;
    }

    return bban_handler->preformat(bban);
}

std::string BBan_handler::trim(std::string const& country, std::string const& bban)
{
    auto bban_handler = BBan_handler_factory::get_instance()->get_by_country(country);
    if (!bban_handler)
    {
        return bban;
    }

    return bban_handler->trim(bban);
}

std::string BBan_handler::format(std::string const& country, std::string const& bban)
{
    auto bban_handler = BBan_handler_factory::get_instance()->get_by_country(country);
    if (!bban_handler)
    {
        return bban;
    }

    return bban_handler->format(bban);
}

size_t BBan_handler::get_bban_shift(std::string const& country) const
{
    // the iban structure defines the positions from after the iban checksum
    // when the bban is also an iban, we must align the positions
    size_t shift = 0;

    switch (get_bban_type(country))
    {
        default:
        case BBan_type::NATIONAL:
            shift = 0;
            break;

        case BBan_type::IBAN_NO_COUTRY:
            shift = 2;
            break;

        case BBan_type::IBAN:
            shift = 4;
            break;
    }

    return shift;
}

// ==========================================================================

std::map<std::string, std::shared_ptr<BBan_handler>> BBan_handler_factory::m_handlers = {};

BBan_handler_factory::BBan_handler_factory()
{
}

BBan_handler_factory* BBan_handler_factory::get_instance()
{
    static unique_ptr<BBan_handler_factory> m_instance(new BBan_handler_factory);
    return m_instance.get();
}

std::shared_ptr<BBan_handler> BBan_handler_factory::get_by_country(std::string const& country_code) const
{
    string country_code_upper;
    transform(country_code.begin(), country_code.end(), std::back_inserter(country_code_upper), ::toupper);

    auto it = m_handlers.find(country_code);

    return (it != m_handlers.end()) ? it->second : nullptr;
}

void BBan_handler_factory::register_handler(std::string const& country, std::shared_ptr<BBan_handler> handler)
{
    m_handlers[country] = handler;
}

} // namespace iban
