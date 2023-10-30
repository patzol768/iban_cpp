
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

bool BBan_handler::is_valid(std::string const& bban) const
{
    return is_valid_length(bban) && is_valid_checksum(bban) && is_valid_bankcode(bban) && is_valid_ext(bban);
}

bool BBan_handler::is_valid_length(std::string const& bban) const
{
    return (bban.size() == m_iban_structure.bban_length);
}

bool BBan_handler::is_valid_checksum(std::string const& bban) const
{
    return false;
}

bool BBan_handler::is_valid_bankcode(std::string const& bban) const
{
    // validates only bank code
    // if country has branch validation, the specific subclass must override this one
    auto bankcode = bban.substr(m_iban_structure.bank_code.first, m_iban_structure.bank_code.second - m_iban_structure.bank_code.first);

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
