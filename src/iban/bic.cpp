/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/bic.h"
#include "iban/country.h"
#include "iban/error.h"

#include <iostream>
#include <map>
#include <regex>

using iban::Bic_repository;
using iban::Iban_error;
using std::map;
using std::reference_wrapper;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::set;
using std::string;
using std::unique_ptr;
using std::vector;

namespace iban
{
static const std::string empty;

Bic::Bic(std::string const& bic, bool allow_invalid)
: m_code(trim(bic))
, m_is_short(bic.size() == 8)
{
    if (m_is_short)
    {
        m_code.append("XXX");
    }

    if (!allow_invalid && !is_valid())
    {
        throw Iban_error("invalid");
    }
}

Bic::Bic(std::string const& country, std::string const& bankcode, bool allow_multiple)
{
    auto bic_list = from_bank_code(country, bankcode);

    if (!bic_list.size())
    {
        throw Iban_error("not found");
    }

    if (!allow_multiple && bic_list.size() > 1)
    {
        throw Iban_error("ambiguous");
    }

    *this = *bic_list.begin();
}

bool Bic::operator==(Bic const& other) const
{
    // Since short codes are stored with "XXX" postfix, a simple
    // comparison correctly matches the short format and the long
    // format where the branch code is "XXX"
    return m_code == other.m_code;
}

bool Bic::operator==(std::string const& s) const
{
    Bic other(s, true);

    return m_code == other.m_code;
}

bool Bic::operator<(Bic const& other) const
{
    return m_code < other.m_code;
}

Bic::operator std::string() const
{
    return (!m_is_short) ? m_code : m_code.substr(0, 8);
}

std::set<Bic> Bic::from_bank_code(std::string const& country, std::string const& bankcode)
{
    auto result = Bic_repository::get_instance()->get_by_country_bankcode(country, bankcode);

    set<Bic> bics;

    for (auto const& entry : result)
    {
        Bic entry_bic(entry.get().bic);

        if (bics.find(entry_bic) == bics.end())
        {
            bics.emplace(entry_bic);
        }
    }

    return bics;
}

bool Bic::is_valid() const
{
    return is_valid_length() && is_valid_structure() && is_valid_country_code();
}

bool Bic::is_valid_length() const
{
    // Only 11, since the 8 long BIC codes are postfixed with XXX;
    return (m_code.size() == 11);
}

bool Bic::is_valid_structure() const
{
    // Always 11 chars
    static const regex re_bic("^[A-Z]{4}[A-Z]{2}[A-Z0-9]{2}[A-Z0-9]{3}$");

    return regex_match(m_code, re_bic);
}

bool Bic::is_valid_country_code() const
{
    return Country_repository::get_instance()->is_country(get_country_code());
}

bool Bic::is_valid_iban_country_code() const
{
    return Country_repository::get_instance()->is_iban_country(get_country_code());
}

bool Bic::is_valid_sepa_country_code() const
{
    return Country_repository::get_instance()->is_sepa_country(get_country_code());
}

bool Bic::is_existing() const
{
    auto result = Bic_repository::get_instance()->get_by_bic(m_code);

    return (result.size() > 0);
}

std::string Bic::get_formatted() const
{
    string result(get_bank_code() + " " + get_country_code() + " " + get_location_code());

    if (!m_is_short)
    {
        result.append(string(" ") + get_branch_code().value());
    }

    return result;
}

std::set<std::string> Bic::get_domestic_bank_codes() const
{
    auto result = Bic_repository::get_instance()->get_by_bic(m_code);

    set<string> strings;

    for (auto const& entry : result)
    {
        if (strings.find(entry.get().bank_code) == strings.end())
        {
            strings.insert(entry.get().bank_code);
        }
    }

    return strings;
}

std::set<std::string> Bic::get_names() const
{
    auto result = Bic_repository::get_instance()->get_by_bic(m_code);

    set<string> strings;

    for (auto const& entry : result)
    {
        if (strings.find(entry.get().name) == strings.end())
        {
            strings.insert(entry.get().name);
        }
    }

    return strings;
}

std::set<std::string> Bic::get_short_names() const
{
    auto result = Bic_repository::get_instance()->get_by_bic(m_code);

    set<string> strings;

    for (auto const& entry : result)
    {
        if (strings.find(entry.get().short_name) == strings.end())
        {
            strings.insert(entry.get().short_name);
        }
    }

    return strings;
}

Bic_type Bic::get_type() const
{
    auto location_code = get_location_code();

    switch (location_code[1])
    {
        case '0':
            return Bic_type::TESTING;

        case '1':
            return Bic_type::PASSIVE;

        case '2':
            return Bic_type::REVERSE_BILLING;

        default:
            break;
    }

    return Bic_type::DEFAULT;
}

std::string Bic::get_short_code() const
{
    if (m_code.size() == 11 && m_code.substr(8, 3) == "XXX")
    {
        return m_code.substr(0, 8);
    }

    return m_code;
}

std::string Bic::get_country() const
{
    auto country = Country_repository::get_instance()->get_by_alpha2(get_country_code());

    return country.has_value() ? country.value().get().name : empty;
}

std::string Bic::get_bank_code() const
{
    return get_part(0, 4).value_or("");
}

std::string Bic::get_country_code() const
{
    return get_part(4, 2).value_or("");
}

std::string Bic::get_location_code() const
{
    return get_part(6, 2).value_or("");
}

std::optional<std::string> Bic::get_branch_code() const
{
    if (m_is_short)
    {
        return {};
    }

    return get_part(8, 3);
}

std::optional<std::string> Bic::get_part(size_t from, size_t len) const
{
    if (from < 0 || len < 1 || m_code.size() < from + len)
    {
        return {};
    }

    return m_code.substr(from, len);
}

std::string Bic::trim(std::string const& str)
{
    // removes all whitespaces
    auto result = regex_replace(str, regex("\\s"), "");

    // make all uppercase
    transform(result.begin(), result.end(), result.begin(), ::toupper);

    return result;
}

std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, Bic const& rhs)
{
    return lhs << rhs.get_short_code();
}

// ==========================================================================

std::vector<Bic_repository_entry> Bic_repository::m_elements = {};
std::multimap<std::string, Bic_repository_entry const* const> Bic_repository::m_by_bic = {};
std::multimap<std::string, Bic_repository_entry const* const> Bic_repository::m_by_short_bic = {};
std::multimap<std::string, Bic_repository_entry const* const> Bic_repository::m_by_code = {};
std::set<std::string> Bic_repository::m_countries = {};

Bic_repository::Bic_repository()
{
}

Bic_repository* Bic_repository::get_instance()
{
    static unique_ptr<Bic_repository> m_instance(new Bic_repository);
    return m_instance.get();
}

std::vector<reference_wrapper<Bic_repository_entry const>> Bic_repository::get_by_bic(std::string const& bic) const
{
    std::vector<reference_wrapper<Bic_repository_entry const>> result;

    switch (bic.size())
    {
        case 8: // short bic
        {
            auto items = m_by_short_bic.equal_range(bic);
            for (auto it = items.first; it != items.second; ++it)
            {
                auto e = it->second;
                result.push_back(*e);
            }
        }
        break;

        case 11: // long bic
        {
            auto items = m_by_bic.equal_range(bic);
            for (auto it = items.first; it != items.second; ++it)
            {
                result.push_back(*(it->second));
            }
        }
        break;

        default:
            break;
    }

    return result;
}

std::vector<reference_wrapper<Bic_repository_entry const>> Bic_repository::get_by_country_bankcode(std::string const& country_code, std::string const& bank_code) const
{
    std::vector<reference_wrapper<Bic_repository_entry const>> result;

    auto code = country_code + ":" + bank_code;

    auto items = m_by_code.equal_range(code);
    for (auto it = items.first; it != items.second; ++it)
    {
        result.push_back(*(it->second));
    }

    return result;
}

bool Bic_repository::has_bank_list(std::string const& country)
{
    return m_countries.find(country) != m_countries.end();
}

void Bic_repository::load(std::function<void(std::vector<Bic_repository_entry>&)> loader)
{
    vector<Bic_repository_entry> elements;
    loader(elements);

    swap(elements, m_elements);

    m_by_bic.clear();
    m_by_code.clear();
    m_by_short_bic.clear();

    for (auto const& element : m_elements)
    {
        auto size = element.bic.size();
        if (size == 8 || size == 11)
        {
            auto bic = (size == 8) ? element.bic + "XXX" : element.bic;
            auto short_bic = bic.substr(0, 8);
            m_by_bic.insert({bic, &element});
            m_by_short_bic.insert({short_bic, &element});
        }

        auto bank_code = element.country_code + ":" + element.bank_code;

        m_by_code.insert({bank_code, &element});
        m_countries.insert(element.country_code);
    }
}

} // namespace iban