#include "iban/countries/CZ_SK.h"
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

BBan_handler_CZ_SK::BBan_handler_CZ_SK(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_CZ_SK::is_valid_checksum(std::string const& bban) const
{
    if (!is_valid_length(bban))
    {
        return false;
    }

    // algorithm defined in https://www.nbs.sk/_img/Documents/_Legislativa/_Vestnik/OPAT8-09.pdf
    auto account = bban.substr(10);

    int sum = 0;
    static const int multiplier[10] = {6, 3, 7, 9, 10, 5, 8, 4, 2, 1};

    for (auto i = 0; i < 10; i++)
    {
        sum += (account[i] - '0') * multiplier[i];
    }

    return sum % 11 == 0;
}

__attribute__((weak)) bool BBan_handler_CZ_SK::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_CZ_SK::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    const regex numeric("^.*([0-9]{20}).*$");
    const regex formatted("^[^0-9]*([0-9]{1,6})-([0-9]{10})/([0-9]{4}).*$");
    const string padding("000000");

    auto trimmed_bban = regex_replace(bban, trim, "");

    // if formatted, then most probably it is in local format
    smatch formatted_result;
    regex_match(trimmed_bban, formatted_result, formatted);

    if (formatted_result.size() == 4)
    {
        auto bank = formatted_result[3].str();
        auto branch = formatted_result[1].str();
        auto account = formatted_result[2].str();

        branch = padding.substr(0, 6 - branch.size()) + branch;

        return bank + branch + account;
    }

    // if full numeric, than already prepared for IBAN creation
    smatch numeric_result;
    regex_match(trimmed_bban, numeric_result, numeric);

    if (numeric_result.size() == 2)
    {
        return numeric_result[1].str();
    }

    return "";
}

std::string BBan_handler_CZ_SK::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_CZ_SK::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 4);
    auto branch = bban.substr(4, 6);
    auto account = bban.substr(10);

    branch.erase(0, min(branch.find_first_not_of('0'), branch.size()));

    auto result = branch + "-" + account + "/" + bank;

    return result;
}

} // namespace countries
} // namespace iban
