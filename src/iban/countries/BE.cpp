#include "iban/countries/BE.h"
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

BBan_handler_BE::BBan_handler_BE(std::string const& country)
: BBan_handler(country)
{
}

bool BBan_handler_BE::is_valid_checksum(std::string const& bban) const
{
    if (!is_valid_length(bban))
    {
        return false;
    }

    auto account = bban.substr(0, 10);
    auto checksum = bban.substr(10);

    // signed 64 bit integer has 19 digits, here we have only 10
    int64_t account_numeric = stoll(account);
    int64_t remainder = account_numeric % 97LL;
    int64_t checksum_numeric = stoll(checksum);

    return remainder == checksum_numeric;
}

__attribute__((weak)) bool BBan_handler_BE::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_BE::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    const regex numeric("^.*([0-9]{12}).*$");
    const regex formatted("^[^0-9]*([0-9]{3})-([0-9]{1,7})-([0-9]{2}).*$");
    const string padding("0000000");

    auto trimmed_bban = regex_replace(bban, trim, "");

    // if formatted, then most probably it is in local format
    smatch formatted_result;
    regex_match(trimmed_bban, formatted_result, formatted);

    if (formatted_result.size() == 4)
    {
        auto bank = formatted_result[1].str();
        auto account = formatted_result[2].str();
        auto checksum = formatted_result[3].str();

        account = padding.substr(0, 7 - account.size()) + account;

        return bank + account + checksum;
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

std::string BBan_handler_BE::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_BE::format(std::string const& bban) const
{
    auto bank = bban.substr(0, 3);
    auto account = bban.substr(3, 7);
    auto checksum = bban.substr(10);

    account.erase(0, min(account.find_first_not_of('0'), account.size()));

    auto result = bank + "-" + account + "-" + checksum;

    return result;
}

} // namespace countries
} // namespace iban
