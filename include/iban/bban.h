#pragma once

#include "common.h"

#include <map>
#include <memory>
#include <string>

namespace iban
{

class BBan_handler
{
    public:
    // Returns the coutry code
    virtual std::string const& get_country() const = 0;

    // Validate the structural identity of the BBAN
    virtual bool is_valid(std::string const& bban) const;

    // Validate if the BBAN's length is valid
    virtual bool is_valid_length(std::string const& bban) const;

    // Validate the checksum(s) inside the BBAN
    virtual bool is_valid_checksum(std::string const& bban) const;

    // Validate the bankcode (if present in the BBAN)
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
};

class BBan_handler_factory
{
    public:
    ~BBan_handler_factory() { }

    static BBan_handler_factory* get_instance();

    // getters

    // Looks for a country specific validator class instance
    // nullptr if not found
    std::shared_ptr<BBan_handler> get_by_country(std::string const& country_code) const;

    // setters

    // Registers a country specific validator
    void register_validator(std::string const& country, std::shared_ptr<BBan_handler> validator);

    private:
    BBan_handler_factory();
    BBan_handler_factory(const BBan_handler_factory&) = delete;                  // copy constructor
    BBan_handler_factory& operator=(const BBan_handler_factory& other) = delete; // assignment operator
    BBan_handler_factory(BBan_handler_factory&& other) = delete;                 // move constructor
    BBan_handler_factory& operator=(BBan_handler_factory&& other) = delete;      // move assignment operator

    static std::map<std::string, std::shared_ptr<BBan_handler>> m_validators;
};

} // namespace iban