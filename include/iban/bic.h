#pragma once

#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace iban
{

enum class Bic_type
{
    DEFAULT,
    TESTING,
    PASSIVE,
    REVERSE_BILLING
};

class Bic
{
    public:
    // Create a BIC from a string value. Bic("GENODEM1GLS")
    Bic(std::string const& bic, bool allow_invalid = false);

    // Create a BIC from a country and a bankcode. Bic("DE", "43060967")
    Bic(std::string const& country, std::string const& bankcode, bool allow_multiple = false);

    // Equality, where short code also equals long code with "XXX" branch code
    bool operator==(Bic const& other) const;

    // Equality, where short code also equals long code with "XXX" branch code
    bool operator==(std::string const& s) const;

    // Ordering
    bool operator<(Bic const& other) const;

    // Converting to string. Returning the original BIC, i.e. neither removes
    // nor adds the "XXX" branch code.
    operator std::string() const;

    // Create a BIC from a country and a bankcode. Bic("DE", "43060967")
    static std::set<Bic> from_bank_code(std::string const& country, std::string const& bankcode);

    // Validate the structural integrity of this BIC.
    bool is_valid() const;

    // Validate length.
    bool is_valid_length() const;

    // Validate structure.
    bool is_valid_structure() const;

    // Validate country code.
    bool is_valid_country_code() const;

    // Validate if IBAN country.
    bool is_valid_iban_country_code() const;

    // Validate if SEPA country.
    bool is_valid_sepa_country_code() const;

    // Check if exist in our list.
    bool is_existing() const;

    // Get BIC parts separated by space.
    std::string get_formatted() const;

    // Get domestic bank codes associated with the BIC.
    std::set<std::string> get_domestic_bank_codes() const;

    // Get bank names associated with the BIC.
    std::set<std::string> get_names() const;

    // Get bank short names associated with the BIC.
    std::set<std::string> get_short_names() const;

    // Get the type of the BIC.
    Bic_type get_type() const;

    // Get short code. If branch code is "XXX", removes. Otherwise returns the stored code.
    std::string get_short_code() const;

    // Get country name. Returns empty if unknown.
    std::string get_country() const;

    // Get bank_code. Returns empty if m_code is too short to extract. Does not validate the value.
    std::string get_bank_code() const;

    // Get country code. Returns empty if m_code is too short to extract. Does not validate the value.
    std::string get_country_code() const;

    // Get location code. Returns empty if m_code is too short to extract. Does not validate the value.
    std::string get_location_code() const;

    // Get branch code. Returns empty if short code was inputted, OR if m_code is too short to extract. Does not validate the value.
    std::optional<std::string> get_branch_code() const;

    // operator<<
    // size()
    // hash??

    private:
    std::optional<std::string> get_part(size_t from, size_t to) const;
    static std::string trim(std::string const& str);

    std::string m_code; // contains the BIC, always on 11 chars
    bool m_is_short;    // marks if the original BIC had 8 chars

    friend std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, Bic const& rhs);
};

std::basic_ostream<char, std::char_traits<char>>& operator<<(std::basic_ostream<char, std::char_traits<char>>& lhs, Bic const& rhs);

class Bic_repository_entry
{
    public:
    Bic_repository_entry() = default;
    ~Bic_repository_entry() = default;
    Bic_repository_entry(const Bic_repository_entry&) = delete;                  // copy constructor
    Bic_repository_entry& operator=(const Bic_repository_entry& other) = delete; // assignment operator
    Bic_repository_entry(Bic_repository_entry&& other) = delete;                 // move constructor
    Bic_repository_entry& operator=(Bic_repository_entry&& other) = delete;      // move assignment operator

    std::string country_code; // "BG",
    std::string bank_code;    // "IORT",
    std::string short_name;   // "INVESTBANK AD",
    std::string name;         // "INVESTBANK AD",
    std::string bic;          // "IORTBGSF",
    bool primary;             // true
};

// Manages the current list of bank information.
class Bic_repository
{
    public:
    Bic_repository();
    ~Bic_repository() { }

    static Bic_repository* get_instance();

    // getters

    // Looks for the given BIC code. Use the "XXX" postfix in case the no branch banks are required.
    // The 8 char long query would return all branches including where there's no branch.
    std::vector<std::reference_wrapper<Bic_repository_entry const>> get_by_bic(std::string const& bic) const;

    // Looks for country and bankcode.
    std::vector<std::reference_wrapper<Bic_repository_entry const>> get_by_country_bankcode(std::string const& country_code, std::string const& bank_code) const;

    // setters

    // Loads the current set of BIC codes. The "no branch" codes must have "XXX" branch code added.
    // TODO: allow multi threading (loader + getter race)
    void load(std::function<void(std::vector<Bic_repository_entry> const&)> loader);

    private:
    Bic_repository(const Bic_repository&) = delete;                  // copy constructor
    Bic_repository& operator=(const Bic_repository& other) = delete; // assignment operator
    Bic_repository(Bic_repository&& other) = delete;                 // move constructor
    Bic_repository& operator=(Bic_repository&& other) = delete;      // move assignment operator

    static std::vector<Bic_repository_entry> m_elements;
    static std::multimap<std::string, Bic_repository_entry const* const> m_by_bic;
    static std::multimap<std::string, Bic_repository_entry const* const> m_by_short_bic;
    static std::multimap<std::string, Bic_repository_entry const* const> m_by_code;
};

} // namespace iban
