/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 *
 * Description for methods: https://www.bankinfrastruktur.se/framtidens-betalningsinfrastruktur/iban-och-svenskt-nationellt-kontonummer
 * Bank list: https://www.bankinfrastruktur.se/media/s0of3yyj/iban-id-och-bic-adress-for-banker-2023-09-07.pdf
 * IBAN conversion: https://www.bankgirot.se/globalassets/dokument/anvandarmanualer/bankernaskontonummeruppbyggnad_anvandarmanual_sv.pdf
 *
 */

#include "iban/countries/SE.h"
#include "iban/bic.h"
#include "iban/common.h"
#include "iban/iban.h"

// #include <log4cplus/logger.h>
// #include <log4cplus/loggingmacros.h>
#include <regex>
#include <vector>

using std::min;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::smatch;
using std::string;
using std::to_string;
using std::vector;

// Type 1: 1234 12 3456 1
// Type 2: 1234 123456789 1

namespace iban
{
namespace countries
{

static const vector<Account_info> account_info = {
{1100, 1199, "300", "NDEASESS", "Nordea", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{1200, 1399, "120", "DABASESX", "Danske Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{1400, 2099, "300", "NDEASESS", "Nordea", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{2300, 2399, "230", "AABASESS", "Ålandsbanken", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{2400, 2499, "120", "DABASESX", "Danske Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{3300, 3300, "300", "NDEASESS", "Nordea (personkto)", Iban_conv_type::METHOD1, Checksum_type::TYPE2_METH1}, // exception to the next line
{3000, 3399, "300", "NDEASESS", "Nordea", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{3400, 3409, "902", "ELLFSESS", "Länsförsäkringar Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{3782, 3782, "300", "NDEASESS", "Nordea (personkto)", Iban_conv_type::METHOD1, Checksum_type::TYPE2_METH1}, // exception to the next line
{3410, 3999, "300", "NDEASESS", "Nordea", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{4000, 4999, "300", "NDEASESS", "Nordea", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{5000, 5999, "500", "ESSESESS", "SEB", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{6000, 6999, "600", "HANDSESS", "Handelsbanken", Iban_conv_type::METHOD2, Checksum_type::TYPE2_METH2},
{7000, 7999, "800", "SWEDSESS", "Swedbank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{8000, 8999, "800", "SWEDSESS", "Swedbank", Iban_conv_type::METHOD3, Checksum_type::TYPE2_METH3},
{9020, 9029, "902", "ELLFSESS", "Länsförsäkringar Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9040, 9049, "904", "CITISESX", "Citibank (filial)", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9060, 9069, "902", "ELLFSESS", "Länsförsäkringar Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9070, 9079, "907", "FEMAMTMT", "Multitude Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9100, 9109, "910", "NNSESES1", "Nordnet Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9120, 9124, "500", "ESSESESS", "SEB", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9130, 9149, "500", "ESSESESS", "SEB", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9150, 9169, "915", "SKIASESS", "Skandiabanken", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9170, 9179, "917", "IKANSE21", "Ikanobanken", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
// 9180 TYPE2_METH1
{9190, 9199, "919", "DNBASESX", "DnB NOR filial", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9230, 9239, "923", "MARGSESS", "Marginalen Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9250, 9259, "925", "SBAVSESS", "SBAB Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
// 9260 TYPE1_METH2
{9270, 9279, "927", "IBCASES1", "ICA Banken", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9280, 9289, "928", "RESUSE21", "Resurs Bank AB", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9300, 9349, "930", "SWEDSESS", "Swedbank", Iban_conv_type::METHOD1, Checksum_type::TYPE2_METH1},
{9390, 9399, "939", "LAHYSESS", "Landshypotek", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9400, 9449, "940", "FORXSES1", "Forex Bank", Iban_conv_type::METHOD1, Checksum_type::UNKNOWN},
{9460, 9469, "946", "BSNOSESS", "Santander Consumer Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9470, 9479, "947", "FTSBSESS", "BNP Paribas", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9500, 9549, "950", "NDEASESS", "Nordea (Plusgirot)", Iban_conv_type::METHOD2, Checksum_type::TYPE2_METH3},
{9550, 9569, "955", "AVANSES1", "Avanza Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9570, 9579, "957", "SPSDSE23", "Sparbanken Syd", Iban_conv_type::METHOD2, Checksum_type::TYPE2_METH1},
{9580, 9589, "958", "BMPBSESS", "AION Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9590, 9599, "959", "ERPFSES2", "Erik Penser Bank AB", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9630, 9639, "963", "LOSADKKK", "Lån & Spar Bank A/S, filial", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9640, 9649, "964", "NOFBSESS", "NOBA Bank (fd. Nordax bank)", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9650, 9659, "965", "MEMMSE21", "MedMera Bank AB", Iban_conv_type::METHOD1, Checksum_type::UNKNOWN},
{9660, 9669, "966", "SVEASES1", "Svea Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9670, 9679, "967", "JAKMSE22", "JAK Medlemsbank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9680, 9689, "968", "BSTPSESS", "Bluestep Finans AB", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH1},
{9700, 9709, "970", "EKMLSE21", "Ekobanken", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9710, 9719, "971", "LUNADK2B", "Lunar Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9750, 9759, "975", "NOHLSESS", "Northmill Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
{9780, 9789, "978", "KLRNSESS", "Klarna Bank", Iban_conv_type::METHOD1, Checksum_type::TYPE1_METH2},
// 9880 TYPE1_METH2
// 9890 TYPE2_METH1
{9960, 9969, "950", "NDEASESS", "Nordea (Plusgirot)", Iban_conv_type::METHOD2, Checksum_type::TYPE2_METH3}};

static const Account_info not_found = {0, 0, "", "", "", Iban_conv_type::UNKNOWN, Checksum_type::UNKNOWN};

BBan_handler_SE::BBan_handler_SE(std::string const& country)
: BBan_handler(country)
{
}

static Account_info const& get_account_info_by_clearing(std::string const& clearing_no)
{
    auto clearing = stoi(clearing_no);
    auto it = std::find_if(account_info.begin(), account_info.end(), [clearing](Account_info const& info)
                           { return (clearing >= info.range_low && clearing <= info.range_high); });

    return it != account_info.end() ? *it : not_found;
}

static Account_info const& get_account_info_by_bank(std::string const& bank_code, std::string const& clearing_no)
{
    auto clearing = stoi(clearing_no);
    auto it = std::find_if(account_info.begin(), account_info.end(), [&bank_code, &clearing](Account_info const& info)
                           { return (bank_code == info.bank_code && clearing >= info.range_low && clearing <= info.range_high); });

    if (it != account_info.end())
    {
        return *it;
    }

    // bank code + clearing code pair not found; this can happen if iban conversion was made by method 2
    it = std::find_if(account_info.begin(), account_info.end(), [&bank_code](Account_info const& info)
                           { return (bank_code == info.bank_code && info.to_iban_method == Iban_conv_type::METHOD2); });

    return it != account_info.end() ? *it : not_found;
}

// works right to left on the digits of the account
static int sum_add_mod(std::string const& account, std::vector<int> const& weights, int modulus, bool master_numbers, size_t first_digit, size_t last_digit)
{
    size_t mod = weights.size();
    int sum = 0;
    size_t i = last_digit + 1;
    size_t p = 0;

    do
    {
        --i;
        auto part = (account[i] - '0') * weights[p];
        if (master_numbers && part > 9)
        {
            // since part is max. 18, hence adding the two digits
            // equal to subtract 9
            part -= 9;
        }
        sum += part;
        ++p;
        p %= mod;
    } while (i > first_digit);

    return sum % modulus;
}

bool check_mod11(std::string const& account, size_t first_digit)
{
    static const vector<int> weights = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto check = sum_add_mod(account, weights, 11, false, first_digit, account.size() - 1);
    return (check == 0);
}

// type 1, method 1 check
static bool checksum_algo_t1m1(std::string const& account)
{
    auto clearing = account.substr(0, 4);
    auto part = account.substr(account.size() <= 11 ? 4 : account.size() - 7, 7);

    return check_mod11(clearing + part, 1);
}

// type 1, method 2 check
static bool checksum_algo_t1m2(std::string const& account)
{
    auto clearing = account.substr(0, 4);
    auto part = account.substr(account.size() <= 11 ? 4 : account.size() - 7, 7);

    return check_mod11(clearing + part, 0);
}

// type 2 check
static bool checksum_algo_t2m1(std::string const& account)
{
    static const vector<int> weights = {1, 2};

    // skip clearing code
    auto sum = sum_add_mod(account, weights, 10, true, account.size() <= 14 ? 4 : account.size() - 10, account.size() - 1);
    return (sum % 10 == 0);
}

// type 2, method 2 check
static bool checksum_algo_t2m2(std::string const& account)
{
    auto first_digit = account.size() < 9 ? 0 : account.size() - 9;
    return check_mod11(account, first_digit);
}

// type 2, method 3 check
static bool checksum_algo_t2m3(std::string const& account)
{
    // same algorithm as in case of type 2 method 1, but
    // in rare cases the checksum validation failes here
    //
    // for now we fail on such rare accounts
    return checksum_algo_t2m1(account);
}

// don't know how to calculate checksum for this account
static bool checksum_algo_unknown(std::string const& account)
{
    return true;
}

static const std::map<Checksum_type, std::function<bool(std::string const&)>> checksum_algorithms = {
{Checksum_type::TYPE1_METH1, checksum_algo_t1m1},
{Checksum_type::TYPE1_METH2, checksum_algo_t1m2},
{Checksum_type::TYPE2_METH1, checksum_algo_t2m1},
{Checksum_type::TYPE2_METH2, checksum_algo_t2m2},
{Checksum_type::TYPE2_METH3, checksum_algo_t2m3},
{Checksum_type::UNKNOWN, checksum_algo_unknown},
};

bool BBan_handler_SE::is_valid_checksum(std::string const& bban) const
{
    auto short_account = bban.substr(m_iban_structure.account_code.first);
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));
    // except if iban conversion method is 2
    auto clearing = short_account.substr(0, 4);

    auto bank_code = bban.substr(m_iban_structure.bank_code.first, m_iban_structure.bank_code.second - m_iban_structure.bank_code.first);

    auto info = get_account_info_by_bank(bank_code, clearing);

    if (info.to_iban_method == Iban_conv_type::UNKNOWN)
    {
        // account is in an undefined range
        return false;
    }

    auto it = checksum_algorithms.find(info.checksum_method);
    if (it != checksum_algorithms.end())
    {
        return it->second(short_account);
    }

    return false;
}

bool BBan_handler_SE::is_valid_bankcode(std::string const& bban) const
{
    return true;
}

__attribute__((weak)) bool BBan_handler_SE::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_SE::preformat(std::string const& bban) const
{
    const regex trim("\\s");
    const regex formatted("^[^0-9]*([0-9]{4})([0-9]{1,12}).*$");

    auto trimmed_bban = regex_replace(bban, trim, "");

    // if formatted, then most probably it is in local format
    smatch formatted_result;
    regex_match(trimmed_bban, formatted_result, formatted);

    if (formatted_result.size() == 3)
    {
        auto clearing_no = formatted_result[1].str();
        auto account = formatted_result[2].str();

        auto info = get_account_info_by_clearing(clearing_no);
        switch (info.to_iban_method)
        {
            case Iban_conv_type::METHOD1:
            case Iban_conv_type::METHOD3:
                account = string(17 - account.size() - clearing_no.size(), '0') + clearing_no + account;
                break;

            case Iban_conv_type::METHOD2:
                account = string(17 - account.size(), '0') + account;
                break;

            default:
                return "";
        }

        account = info.bank_code + account;

        return account;
    }

    return "";
}

std::string BBan_handler_SE::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_SE::format(std::string const& bban) const
{
    auto short_account = bban.substr(m_iban_structure.account_code.first);
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));
    // except if iban conversion method is 2
    auto clearing = short_account.substr(0, 4);

    auto bank_code = bban.substr(m_iban_structure.bank_code.first, m_iban_structure.bank_code.second - m_iban_structure.bank_code.first);
    auto info = get_account_info_by_bank(bank_code, clearing);

    string account;
    string account_2;

    switch (info.to_iban_method)
    {
        case Iban_conv_type::METHOD1:
            clearing = bban.substr(0, 4);
            account = bban.substr(4, 4) + " " + bban.substr(8);
            break;

        case Iban_conv_type::METHOD2:
            clearing = "????"; // Clearing was dropped, when IBAN was created
            account = bban.substr(0, 4) + " " + bban.substr(4);
            break;

        case Iban_conv_type::METHOD3:
            clearing = bban.substr(0, 4) + "-" + bban.substr(4, 1) + ",";
            account = bban.substr(5, 3) + " " + bban.substr(8, 3) + " " + bban.substr(11, 3) + "-" + bban.substr(14, 1);
            break;

        default:
            return "";
    }

    return clearing + " " + account;
}

} // namespace countries
} // namespace iban
