#include "iban/countries/BG.h"
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

BBan_handler_BG::BBan_handler_BG(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_BG::is_valid_checksum(std::string const& bban) const
{
    // according to the official account number formatting rules
    // there are no country specific checksum
    //
    // https://www.bnb.bg/bnbweb/groups/public/documents/bnb_law/regulations_iban_en.pdf
    return true;
}

__attribute__((weak)) bool BBan_handler_BG::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_BG::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    // space is used as separator, but trimming removes
    const regex alphanumeric("^.*([0-9A-Z]{4}[0-9]{4}[0-9]{2}[0-9A-Z]{8}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    smatch alnum_result;
    regex_match(trimmed_bban, alnum_result, alphanumeric);

    if (alnum_result.size() == 2)
    {
        return alnum_result[1].str();
    }

    return "";
}

std::string BBan_handler_BG::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_BG::format(std::string const& bban) const
{
    auto result = bban.substr(0, 4);

    for (size_t p = 4; p < bban.size(); p += 4)
    {
        result += " " + bban.substr(p, 4);
    }

    return result;
}

} // namespace countries
} // namespace iban
