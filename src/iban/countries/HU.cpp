#include "iban/countries/HU.h"
#include "iban/bic.h"

#include <regex>

using std::regex;
using std::regex_replace;
using std::string;

namespace iban
{
namespace countries
{

BBan_handler_HU::BBan_handler_HU(std::string const& country)
:BBan_handler(country)
{
}

bool BBan_handler_HU::is_valid_checksum(std::string const& bban) const
{
    if (!is_valid_length(bban))
    {
        return false;
    }

    // not checking the bankcode's checksum, since all codes are coming from a list

    // algorithm defined by 18/2009 regulation of the Nation Bank of Hungary
    auto account = bban.substr(8);

    if (account.size() == 8)
    {
        account.append("00000000");
    }

    int sum = 0;
    static const int multiplier[4] = {9, 7, 3, 1};

    for (auto i = 0; i < 16; i++)
    {
        sum += (account[i] - '0') * multiplier[i % 4];
    }

    return sum % 10 == 0;
}

bool BBan_handler_HU::is_valid_bankcode(std::string const& bban) const
{
    // bank + branch + check digit
    auto bankcode = bban.substr(0, 8);

    auto bank = Bic_repository::get_instance()->get_by_country_bankcode(m_country, bankcode);

    return bank.size() != 0;
}

__attribute__((weak)) bool BBan_handler_HU::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_HU::preformat(std::string const& bban) const
{
    // remove any separators and not allowed chars
    auto temp_bban = regex_replace(bban, regex("[^0-9]"), "");

    switch(temp_bban.size())
    {
        case 16:
            return temp_bban + "00000000";

        case 24:
            return temp_bban;

        default:;
    }

    return "";
}

std::string BBan_handler_HU::trim(std::string const& bban) const
{
    if (bban.size() > 8 && bban.substr(bban.size() - 8) == "00000000")
    {
        return bban.substr(0, bban.size() - 8);
    }

    return bban;
}

std::string BBan_handler_HU::format(std::string const& bban) const
{
    if (bban.size() != 16 && bban.size() != 24)
    {
        return bban;
    }

    auto result = bban.substr(0, 8) + "-" + bban.substr(8, 8);

    if (bban.size() == 24)
    {
        result += "-" + bban.substr(16, 8);
    }

    return result;
}

} // namespace countries
} // namespace iban
