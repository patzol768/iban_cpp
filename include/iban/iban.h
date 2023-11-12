/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */ 

#pragma once

#include "common.h"

#include <functional>
#include <map>
#include <optional>
#include <string>

namespace iban
{
class Iban_structure_entry;

class Iban
{
    public:
    Iban(std::string const& iban, bool allow_invalid, bool validate_bban);
    Iban(std::string const& country_code, std::string const& bban, bool validate_bban);
    Iban(std::string const& country_code, std::string const& bank_code, std::string const& branch_code, std::string const& account_code, bool validate_bban);
    ~Iban() = default;
    Iban(const Iban&) = delete;                  // copy constructor
    Iban& operator=(const Iban& other) = delete; // assignment operator
    Iban(Iban&& other) = default;                // move constructor
    Iban& operator=(Iban&& other) = default;     // move assignment operator

    // Validate the structural integrity of this BIC.
    bool is_valid() const;

    // Validate length.
    bool is_valid_length() const;

    // Validate structure. If it contains only allowed chars.
    bool is_valid_structure() const;

    // Validate country code.
    bool is_valid_country_code() const;

    // Validates the iban checksum
    bool is_valid_iban_checksum() const;

    // Validates the bban
    bool is_valid_bban() const;

    // Returns the country code from the IBAN
    std::string get_country_code() const;

    // Returns the IBAN checksum
    std::string get_iban_checksum() const;

    // Returns the national bank code
    std::string get_bankcode() const;

    // Returns the national branch code (emtpy if no branch info)
    std::string get_branchcode() const;

    // Returns the account identifier inside the bank+branch
    std::string get_account() const;

    // Returns the BBAN from the IBAN
    std::string get_bban() const;

    // Returns the BBAN from the IBAN, in short format if possible
    std::string get_bban_t() const;

    // Returns the BBAN from the IBAN, formatted for presentation
    std::string get_bban_f() const;

    // Returns the BBAN from the IBAN, formatted for presentation, in short format if possible
    std::string get_bban_tf() const;

    // ISO7064 checksum algorithm algorithm (mod 97 10)
    static std::string checksum_mod97(std::string const& s);

    // Converts the account number to the numeric string described in the checksum algorithm (iso7064)
    static std::string to_numeric(std::string const& s);

    private:
    std::string iban_checksum(std::string const& country_code, std::string const& bban) const;
    void set_iban(std::string const& country_code, std::string const& bban, bool validate_bban);
    void set_bban_position();

    std::string m_iban;
    size_t m_bban_position; // on what position of the iban is the bban starting (0/2/4)
    const Iban_structure_entry& m_iban_structure;

    friend std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, Iban const& rhs);
};

std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, Iban const& rhs);

class Iban_structure_entry
{
    public:
    Iban_structure_entry() = default;
    ~Iban_structure_entry() = default;
    Iban_structure_entry(const Iban_structure_entry&) = delete;                  // copy constructor
    Iban_structure_entry& operator=(const Iban_structure_entry& other) = delete; // assignment operator
    Iban_structure_entry(Iban_structure_entry&& other) = default;                // move constructor
    Iban_structure_entry& operator=(Iban_structure_entry&& other) = default;     // move assignment operator

    void override(Iban_structure_entry const& v);

    std::string bban_spec;                  // 4!n4!n12!c
    std::string iban_spec;                  // "AD2!n4!n4!n12!c"
    size_t bban_length;                     // 20
    size_t iban_length;                     // 24
    std::pair<size_t, size_t> account_code; // <8, 20> <first character position inclusice, last character position exclusive>
    std::pair<size_t, size_t> bank_code;    // <0, 4>
    std::pair<size_t, size_t> branch_code;  // <4, 8>
    bool overridden;                        // false
};

// Manages the current list of bank information.
class Iban_structure_repository
{
    public:
    ~Iban_structure_repository() { }

    static Iban_structure_repository* get_instance();

    // getters

    // Looks for iban structucre definition for the given country
    // Throws if no definition for the named country
    Iban_structure_entry const& get_by_country(std::string const& country_code) const;

    // setters

    // Loads the current set of BIC codes. The "no branch" codes must have "XXX" branch code added.
    // TODO: allow multi threading (loader + getter race)
    void load(std::function<void(std::map<std::string, Iban_structure_entry>&)> loader);

    private:
    Iban_structure_repository();
    Iban_structure_repository(const Iban_structure_repository&) = delete;                  // copy constructor
    Iban_structure_repository& operator=(const Iban_structure_repository& other) = delete; // assignment operator
    Iban_structure_repository(Iban_structure_repository&& other) = delete;                 // move constructor
    Iban_structure_repository& operator=(Iban_structure_repository&& other) = delete;      // move assignment operator

    static std::map<std::string, Iban_structure_entry> m_elements;
};


} // namespace iban
