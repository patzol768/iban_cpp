#pragma once

#include <functional>
#include <map>
#include <string>
#include <optional>

namespace iban
{
class Country
{
    public:
    // Converting to string. Returning the country name.
    operator std::string() const;

    std::string id;
    std::string a3;
    std::string num;
    std::string name;
    bool independent;
    bool non_iso;
    bool use_iban;
    bool is_sepa;
};

class Country_repository
{
    public:
    Country_repository();
    ~Country_repository() { }

    static Country_repository* get_instance();

    // getters

    // Returns the data of the given country by its iso3166 alpha-2 code.
    std::optional<std::reference_wrapper<Country const>> get_by_alpha2(std::string const& code) const;

    // Checks if country exists by its iso3166 alpha-2 code.
    bool is_country(std::string const& code) const;

    // Checks if country is an iban using country by its iso3166 alpha-2 code.
    bool is_iban_country(std::string const& code) const;

    // Checks if country is a sepa country by its iso3166 alpha-2 code.
    bool is_sepa_country(std::string const& code) const;

    // setters

    // Loads the current set of BIC codes. The "no branch" codes must have "XXX" branch code added.
    // TODO: allow multi threading (loader + getter race)
    void load(std::function<void(std::map<std::string, Country> const&)> loader);

    private:
    Country_repository(const Country_repository&) = delete;                  // copy constructor
    Country_repository& operator=(const Country_repository& other) = delete; // assignment operator
    Country_repository(Country_repository&& other) = delete;                 // move constructor
    Country_repository& operator=(Country_repository&& other) = delete;      // move assignment operator

    static std::map<std::string, Country> m_countries;
};


} // namespace iban