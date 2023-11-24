/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/iban.h"
#include "iban/bban.h"
#include "iban/country.h"
#include "iban/error.h"

#include <iostream>
#include <memory>
#include <regex>

using iban::Country_repository;
using iban::Iban_error;
using iban::Iban_structure_entry;
using iban::Iban_structure_repository;
using std::map;
using std::pair;
using std::reference_wrapper;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::string;
using std::to_string;
using std::unique_ptr;

namespace iban
{

// removes non-iban allowed chars, the rest shall contain on IBAN conform chars
Iban::Iban(std::string const& iban, bool allow_invalid, bool validate_bban)
: m_iban_structure(Iban_structure_repository::get_instance()->get_by_country(iban.substr(0, 2)))
, m_iban("")
, m_bban_position(4)
{
    // no ::toupper conversion, since IBAN standard defines the use of capital letters

    // remove any separators and not allowed chars
    m_iban = regex_replace(iban, regex("[^0-9A-Z]"), "");

    if (!allow_invalid && !is_valid())
    {
        throw Iban_error("invalid iban");
    }

    set_bban_position();

    if (validate_bban && !is_valid_bban())
    {
        throw Iban_error("invalid bban");
    }
}

// allows small letters in bban, but would later translate to IBAN conform capital letters
Iban::Iban(std::string const& country_code, std::string const& bban, bool validate_bban)
: m_iban_structure(Iban_structure_repository::get_instance()->get_by_country(country_code))
, m_iban("")
, m_bban_position(4)
{
    set_iban(country_code, bban, validate_bban);
}

// not allows separators in any of the inputs
Iban::Iban(std::string const& country_code, std::string const& bank_code, std::string const& branch_code, std::string const& account_code, bool validate_bban)
: m_iban_structure(Iban_structure_repository::get_instance()->get_by_country(country_code))
, m_iban("")
, m_bban_position(4)
{
    if (bank_code.size() != m_iban_structure.bank_code.second - m_iban_structure.bank_code.first)
    {
        throw Iban_error("invalid bank code size");
    }

    if (account_code.size() > m_iban_structure.account_code.second - m_iban_structure.account_code.first)
    {
        // leading zeros are frequently left in case of account numbers, hence we don't expect equal length
        throw Iban_error("invalid account code size");
    }

    if (branch_code.size() != m_iban_structure.branch_code.second - m_iban_structure.branch_code.first)
    {
        throw Iban_error("invalid branch code size");
    }

    string bban(m_iban_structure.bban_length, '0');
    bban.replace(m_iban_structure.bank_code.first, bank_code.size(), bank_code);
    bban.replace(m_iban_structure.account_code.second - account_code.size(), account_code.size(), account_code);
    bban.replace(m_iban_structure.branch_code.first, branch_code.size(), branch_code);

    // TODO: In PT if account number present separately, than we have to calculate its check digit and append
    set_iban(country_code, bban, validate_bban);
}

void Iban::set_iban(std::string const& country_code, std::string const& bban, bool validate_bban)
{
    std::string temp_bban;
    transform(bban.begin(), bban.end(), std::back_inserter(temp_bban), ::toupper);

    auto handler = BBan_handler_factory::get_instance()->get_by_country(country_code);
    auto formatted_bban = (handler) ? handler->preformat(temp_bban) : temp_bban;

    if (formatted_bban.empty())
    {
        throw Iban_error("invalid bban");
    }

    if (!handler || handler->get_bban_type() == BBan_type::NATIONAL)
    {
        string check = iban_checksum(country_code, formatted_bban);

        m_iban = check + formatted_bban;

        if (validate_bban && !is_valid_bban())
        {
            throw Iban_error("invalid bban");
        }
    }
    else
    {
        if (handler->get_bban_type() == BBan_type::IBAN)
        {
            m_iban = formatted_bban;
        }
        else
        {
            m_iban = country_code + formatted_bban;
        }

        // this country uses IBAN as local format, hence we run also IBAN validation
        // (BBAN validation is only here to accidentally check some "old" checksums)
        if (validate_bban && !is_valid_bban() && !is_valid())
        {
            throw Iban_error("invalid iban");
        }
    }

    set_bban_position();
}

void Iban::set_bban_position()
{
    auto handler = BBan_handler_factory::get_instance()->get_by_country(get_country_code());
    auto bban_type = (handler) ? handler->get_bban_type() : BBan_type::NATIONAL;

    switch (bban_type)
    {
        default:
        case BBan_type::NATIONAL:
            m_bban_position = 4;
            break;

        case BBan_type::IBAN_NO_COUTRY:
            m_bban_position = 2;
            break;

        case BBan_type::IBAN:
            m_bban_position = 0;
            break;
    }
}

bool Iban::is_valid() const
{
    return is_valid_structure() && is_valid_country_code() && is_valid_length() && is_valid_iban_checksum();
}

bool Iban::is_valid_length() const
{
    if (m_iban.size() < 5 || m_iban.size() > 34)
    {
        return false;
    }

    auto& iban_structure = Iban_structure_repository::get_instance()->get_by_country(get_country_code());

    return (iban_structure.iban_length == m_iban.size());
}

bool Iban::is_valid_structure() const
{
    // Must begin with 2 alpha followed by 2 numeric
    static const regex re_iban_chek("^[A-Z]{2}[0-9]{2}.*$");

    return regex_match(m_iban, re_iban_chek);
}

bool Iban::is_valid_country_code() const
{
    return Country_repository::get_instance()->is_iban_country(get_country_code());
}

bool Iban::is_valid_iban_checksum() const
{
    string check = iban_checksum(get_country_code(), m_iban.substr(4));

    return (m_iban.substr(0, 4) == check);
}

bool Iban::is_valid_bban() const
{
    auto handler = BBan_handler_factory::get_instance()->get_by_country(get_country_code());

    if (handler)
    {
        // have country specific handler
        return handler->is_valid(get_bban());
    }

    // check lengths as registered in IBAN spec - not much, but cannot do more
    auto& iban_structure = Iban_structure_repository::get_instance()->get_by_country(get_country_code());

    return get_bban().size() == iban_structure.bban_length;
}

std::string Iban::get_iban() const
{
    return m_iban;
}

std::string Iban::get_iban_f() const
{
    string formatted;
    auto iban_size = m_iban.size();

    for (size_t p = 0; p < iban_size; p += 4)
    {
        formatted += m_iban.substr(p, 4);

        if (p + 4 < iban_size)
        {
            formatted += ' ';
        }
    }

    return formatted;
}

std::string Iban::get_country_code() const
{
    return m_iban.substr(0, 2);
}

std::string Iban::get_iban_checksum() const
{
    return m_iban.substr(2, 2);
}

std::string Iban::get_bankcode() const
{
    return m_iban.substr(m_iban_structure.bank_code.first + m_bban_position, m_iban_structure.bank_code.second - m_iban_structure.bank_code.first);
}

std::string Iban::get_branchcode() const
{
    return m_iban.substr(m_iban_structure.branch_code.first + m_bban_position, m_iban_structure.branch_code.second - m_iban_structure.branch_code.first);
}

std::string Iban::get_account() const
{
    return m_iban.substr(m_iban_structure.account_code.first + m_bban_position, m_iban_structure.account_code.second - m_iban_structure.account_code.first);
}

std::string Iban::get_bban() const
{
    if (m_iban.size() < m_bban_position)
    {
        return "";
    }

    return m_iban.substr(m_bban_position);
}

std::string Iban::get_bban_t() const
{
    return BBan_handler::trim(get_country_code(), get_bban());
}

std::string Iban::get_bban_f() const
{
    return BBan_handler::format(get_country_code(), get_bban());
}

std::string Iban::get_bban_tf() const
{
    auto bban_handler = BBan_handler_factory::get_instance()->get_by_country(get_country_code());
    if (!bban_handler)
    {
        return get_bban();
    }

    return bban_handler->format(bban_handler->trim(get_bban()));
}

// expects trimmed iban (contains valid iban chars only)
std::string Iban::to_numeric(std::string const& s)
{
    string result;

    for (auto it = s.begin(); it != s.end(); ++it)
    {
        if (*it >= '0' && *it <= '9')
        {
            result.push_back(*it);
        }
        else
        {
            result += to_string(*it - 'A' + 10);
        }
    }

    return result;
}

std::string Iban::checksum_mod97(std::string const& s)
{
    auto numeric = to_numeric(s);

    size_t p = 0;
    size_t digits = 16; // signed 64 bit integer has 19 digits, so to avoid overflow use 16 + 2 (remainder)
    int64_t remainder = 0;

    while (p < numeric.size())
    {
        string s_segment = to_string(remainder) + numeric.substr(p, digits);
        int64_t i_segment = stoll(s_segment);
        remainder = i_segment % 97LL;

        p += digits;
    }

    remainder = 98 - remainder;

    return (remainder < 10) ? "0" + to_string(remainder) : to_string(remainder);
}

// expects trimmed bban (contains valid iban chars only), captial letters in country code
std::string Iban::iban_checksum(std::string const& country_code, std::string const& bban) const
{
    return country_code + checksum_mod97(bban + country_code + "00");
}

std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, Iban const& rhs)
{
    return lhs << rhs.get_iban_f();
}

// ==========================================================================

void Iban_structure_entry::override(Iban_structure_entry const& v)
{
    if (!v.bban_spec.empty())
        bban_spec = v.bban_spec;

    if (!v.iban_spec.empty())
        iban_spec = v.iban_spec;

    if (v.bban_length > 0)
        bban_length = v.bban_length;

    if (v.iban_length > 0)
        iban_length = v.iban_length;

    // if any of the sizes defined, overwrite all values
    if (v.account_code.second > 0 || v.bank_code.second > 0 || v.branch_code.second > 0)
    {
        account_code = v.account_code;
        bank_code = v.bank_code;
        branch_code = v.branch_code;
    }

    overridden = true;
}

// ==========================================================================

const Iban_structure_entry empty_structure = {"", "", 0, 0, {0, 0}, {0, 0}, {0, 0}, false};

// ==========================================================================

map<string, Iban_structure_entry> Iban_structure_repository::m_elements = {};

Iban_structure_repository::Iban_structure_repository()
{
}

Iban_structure_repository* Iban_structure_repository::get_instance()
{
    static unique_ptr<Iban_structure_repository> m_instance(new Iban_structure_repository);
    return m_instance.get();
}

Iban_structure_entry const& Iban_structure_repository::get_by_country(std::string const& country_code) const
{
    auto it = m_elements.find(country_code);
    if (it == m_elements.end())
    {
        return empty_structure;
    }

    return it->second;
}

void Iban_structure_repository::load(std::function<void(std::map<std::string, Iban_structure_entry>&)> loader)
{
    map<string, Iban_structure_entry> elements;
    loader(elements);

    swap(elements, m_elements);
}

} // namespace iban
