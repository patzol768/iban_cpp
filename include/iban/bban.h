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
#include "iban.h"

#include <map>
#include <memory>
#include <string>

namespace iban
{

class BBan_handler
{
    public:
    BBan_handler(std::string const& country);

    // Returns the coutry code
    virtual std::string const& get_country() const final;

    // Validate the structural integrity of the BBAN
    virtual bool is_valid(std::string const& bban) const;

    // Validate if the BBAN's length is valid. Note that bban must be preformatted for IBAN.
    virtual bool is_valid_length(std::string const& bban) const;

    // Validate the checksum(s) inside the BBAN
    virtual bool is_valid_checksum(std::string const& bban) const;

    // Validate the bankcode and possibly branch code
    virtual bool is_valid_bankcode(std::string const& bban) const;

    // Validate the bban with an externally provided validator (e.g. bank specific check)
    virtual bool is_valid_ext(std::string const& bban) const;

    // Makes BBAN suitable for IBAN (e.g. if there are short BBAN formats, etc.)
    virtual std::string preformat(std::string const& bban) const;

    // Cut short the BBAN, if it has both long and short format;
    virtual std::string trim(std::string const& bban) const;

    // Formats BBAN according to the local presentation
    virtual std::string format(std::string const& bban) const;

    // Runs the country specific validation (false if no validation for the given country)
    static bool is_valid(std::string const& country, std::string const& bban);

    // Runs the country specific BBAN's length validation
    static bool is_valid_length(std::string const& country, std::string const& bban);

    // Runs the country specific BBAN checksum(s) validation
    static bool is_valid_checksum(std::string const& country, std::string const& bban);

    // Runs the country specific bankcode validation (if present in the BBAN)
    static bool is_valid_bankcode(std::string const& country, std::string const& bban);

    // Runs the country specific bban validation with an externally provided validator (e.g. bank specific check)
    static bool is_valid_ext(std::string const& country, std::string const& bban);

    // Runs the country specific preformatting (returns unchanged if no country specific function exist)
    static std::string preformat(std::string const& country, std::string const& bban);

    // Runs the country specific trimming (returns unchanged if no country specific function exist)
    static std::string trim(std::string const& country, std::string const& bban);

    // Runs the country specific formatting (returns unchanged if no country specific function exist)
    static std::string format(std::string const& country, std::string const& bban);

    // Divides BBAN into its parts and returns in a map
    // Elements:
    //   - bank
    //   - branch
    //   - account
    //   - nationalchecksum
    static std::map<std::string, std::string> explode(std::string const& country, std::string const& bban); // TODO

    protected:
    const std::string m_country;
    const Iban_structure_entry& m_iban_structure;
};

class BBan_handler_factory
{
    public:
    ~BBan_handler_factory() { }

    static BBan_handler_factory* get_instance();

    // getters

    // Looks for a country specific handler class instance
    // nullptr if not found
    std::shared_ptr<BBan_handler> get_by_country(std::string const& country_code) const;

    // setters

    // Registers a country specific handler
    void register_handler(std::string const& country, std::shared_ptr<BBan_handler> handler);

    private:
    BBan_handler_factory();
    BBan_handler_factory(const BBan_handler_factory&) = delete;                  // copy constructor
    BBan_handler_factory& operator=(const BBan_handler_factory& other) = delete; // assignment operator
    BBan_handler_factory(BBan_handler_factory&& other) = delete;                 // move constructor
    BBan_handler_factory& operator=(BBan_handler_factory&& other) = delete;      // move assignment operator

    static std::map<std::string, std::shared_ptr<BBan_handler>> m_handlers;
};

} // namespace iban
