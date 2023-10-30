#include "iban/countries/DE.h"
#include "iban/bic.h"
#include "iban/common.h"
#include "iban/error.h"

#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
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

namespace iban
{
namespace countries
{

BBan_handler_DE::BBan_handler_DE(std::string const& country)
: BBan_handler(country)
{
}

// works right to left on the digits of the account
static int sum_add_mod(std::string const& account, std::vector<int> const& weights, int modulus, size_t first_digit = 0, size_t last_digit = 8)
{
    size_t mod = weights.size();
    int sum = 0;
    size_t i = last_digit + 1;
    size_t p = 0;

    do
    {
        --i;
        sum += (account[i] - '0') * weights[p];
        ++p;
        p %= weights.size();
    } while (i > first_digit);

    auto checksum = modulus - (sum % modulus);

    // in case of mod 11, the checksum could be 10, which is invalid,
    // meaning that the account number resulting with such checksum
    // is not usable

    return (checksum != modulus) ? checksum : 0;
}

// works left to right on the digits of the account
static int sum_add_mod_master(std::string const& account, std::vector<int> const& weights, std::vector<bool> const& master_numbers,
                              int modulus, bool sum_master_num = false, size_t first_digit = 0, size_t last_digit = 8, int sum_shift = 0)
{
    // weights.size() and master_numbers.size() must be equal

    size_t mod = weights.size();
    int sum = 0;
    size_t i = first_digit;
    size_t p = 0;

    do
    {
        if (master_numbers[p])
        {
            // master number is the sum of the digits of the weighted digit
            auto digit_weight = (account[i] - '0') * weights[p];
            sum += digit_weight / 10 + digit_weight % 10;
        }
        else
        {
            sum += (account[i] - '0') * weights[p];
        }

        ++i;
        ++p;
        p %= weights.size();
    } while (i < last_digit);

    sum += sum_shift;

    if (sum_master_num)
    {
        sum = sum / 10 + sum % 10;
    }

    auto checksum = modulus - (sum % modulus);

    // in case of mod 11, the checksum could be 10, which is invalid,
    // meaning that the account number resulting with such checksum
    // is not usable

    return (checksum != modulus) ? checksum : 0;
}

static int sum_mapper(std::string const& account)
{
    // M10H
    static const vector<vector<int>> mapping = {
    {0, 1, 5, 9, 3, 7, 4, 8, 2, 6}, // zeile 1
    {0, 1, 7, 6, 9, 8, 3, 2, 5, 4}, // zeile 2
    {0, 1, 8, 4, 6, 2, 9, 5, 7, 3}, // zeile 3
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}  // zeile 4
    };

    int sum = 0;
    size_t p = 9;
    do
    {
        --p;
        sum += mapping[p % 4][account[p] - '0'];
    } while (p > 0);

    return 10 - (sum % 10);
}

static bool check_checksum(std::string const& account, int calc_checksum, size_t checksum_digit = 9)
{
    auto checksum = account[checksum_digit] - '0';

    return (checksum == calc_checksum);
}

// 00
static bool checksum_algo_00(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 01
static bool checksum_algo_01(std::string const& account)
{
    static const vector<int> weights = {3, 7, 1};

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 02
static bool checksum_algo_02(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 03
static bool checksum_algo_03(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 04
static bool checksum_algo_04(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 05
static bool checksum_algo_05(std::string const& account)
{
    static const vector<int> weights = {7, 3, 1};

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 06
static bool checksum_algo_06(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 07
static bool checksum_algo_07(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 08
// TODO: min account code check (60000)
static bool checksum_algo_08(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 09
static bool checksum_algo_09(std::string const& account)
{
    return true;
}

// 10
static bool checksum_algo_10(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 11
static bool checksum_algo_11(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto check = sum_add_mod(account, weights, 11);
    if (check == 0)
    {
        check = 9;
    }

    return check_checksum(account, check);
}

// 13
static bool checksum_algo_13(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    return check_checksum(account, sum_add_mod(account, weights, 10, 1, 6), 7);
}

// 14
// not assigned to any bank
static bool checksum_algo_14(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    return check_checksum(account, sum_add_mod(account, weights, 11, 3, 8), 9);
}

// 15
// not assigned to any bank
static bool checksum_algo_15(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5};

    return check_checksum(account, sum_add_mod(account, weights, 11, 5, 8), 9);
}

// 16
static bool checksum_algo_16(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    auto check = sum_add_mod(account, weights, 11);

    if (check = 10)
    {
        return (account[8] == account[9]);
    }

    return check_checksum(account, check);
}

// 17
static bool checksum_algo_17(std::string const& account)
{
    static const vector<int> weights = {1, 2, 1, 2, 1, 2};
    static const vector<bool> master_numbers = {false, true, false, true, false, true};

    auto check = sum_add_mod_master(account, weights, master_numbers, 11, false, 1, 6, -1);

    return check_checksum(account, check, 7);
}

// 18
static bool checksum_algo_18(std::string const& account)
{
    static const vector<int> weights = {3, 9, 7, 1};

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 19
static bool checksum_algo_19(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 1};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 20
static bool checksum_algo_20(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 3};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 21
static bool checksum_algo_21(std::string const& account)
{
    static const vector<int> weights = {2, 1};
    static const vector<bool> master_numbers = {true, true};

    auto check = sum_add_mod_master(account, weights, master_numbers, 10, true);

    return check_checksum(account, check);
}

// 22
static bool checksum_algo_22(std::string const& account)
{
    static const vector<int> weights = {3, 1};

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 23
// not assigned to any bank
static bool checksum_algo_23(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    auto check = sum_add_mod(account, weights, 11, 0, 5);

    if (check = 10)
    {
        return (account[8] == account[9]);
    }

    return check_checksum(account, check, 6);
}

// 24
static bool checksum_algo_24(std::string const& account)
{
    static const vector<int> weights = {1, 2, 3};

    auto begin = 0;
    if (account[0] >= '3' && account[0] <= '6')
    {
        begin = 1;
    }
    else if (account[0] == '9')
    {
        begin = 3;
    }

    auto short_account = account.substr(begin, 9 - begin);
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));

    int sum = 0;
    for (size_t i = 0; i < short_account.size(); ++i)
    {
        sum += ((short_account[i] + 1) * weights[i % weights.size()]) % 11;
    }

    auto check = sum % 10;

    return check_checksum(account, check);
}

// 25
static bool checksum_algo_25(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 0};

    auto check = sum_add_mod(account, weights, 11);

    if (check == 10)
    {
        return (account[9] == '0' && (account[1] == '8' || account[1] == '9'));
    }

    return check_checksum(account, check);
}

// 26
static bool checksum_algo_26(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    auto shifted_account = (account[0] == '0' || account[1] == '0') ? account.substr(2) + "00" : account;
    auto check = sum_add_mod(shifted_account, weights, 11, 0, 6);

    return check_checksum(account, check, 7);
}

// 27
static bool checksum_algo_27(std::string const& account)
{
    auto check = 0;
    if (account[0] == '0')
    {
        static const vector<int> weights = {2, 1};

        check = sum_add_mod(account, weights, 10);
    }
    else
    {
        // M10H
        check = sum_mapper(account);
    }

    return check_checksum(account, check);
}

// 28
static bool checksum_algo_28(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8};

    auto check = sum_add_mod(account, weights, 11, 0, 6);

    if (check == 10)
    {
        check = 0;
    }

    return check_checksum(account, check, 7);
}

// 29
static bool checksum_algo_29(std::string const& account)
{
    return check_checksum(account, sum_mapper(account));
}

// 30
static bool checksum_algo_30(std::string const& account)
{
    static const vector<int> weights = {2, 1, 2, 1, 0, 0, 0, 0, 2};

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 31
static bool checksum_algo_31(std::string const& account)
{
    static const vector<int> weights = {9, 8, 7, 6, 5, 4, 3, 2, 1};

    auto check = sum_add_mod(account, weights, 11);

    if (check == 10)
    {
        check = 0;
    }

    return check_checksum(account, check);
}

// 32
static bool checksum_algo_32(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    return check_checksum(account, sum_add_mod(account, weights, 11, 3, 8), 9);
}

// 33
static bool checksum_algo_33(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6};

    return check_checksum(account, sum_add_mod(account, weights, 11, 4, 8), 9);
}

// 34
static bool checksum_algo_34(std::string const& account)
{
    static const vector<int> weights = {2, 4, 8, 5, 10, 9, 7};

    auto check = sum_add_mod(account, weights, 11, 0, 6);

    if (check == 10)
    {
        check = 0;
    }

    return check_checksum(account, check, 7);
}

// 35
// not assigned to any bank
static bool checksum_algo_35(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto check = sum_add_mod(account, weights, 11);

    if (check == 10)
    {
        return (account[1] == '8' || account[1] == '9');
    }

    return check_checksum(account, check, 7);
}

// 36
// not assigned to any bank
static bool checksum_algo_36(std::string const& account)
{
    static const vector<int> weights = {2, 4, 8, 5};

    return check_checksum(account, sum_add_mod(account, weights, 11, 5, 8), 9);
}

// 37
// not assigned to any bank
static bool checksum_algo_37(std::string const& account)
{
    static const vector<int> weights = {2, 4, 8, 5, 10};

    return check_checksum(account, sum_add_mod(account, weights, 11, 4, 8), 9);
}

// 38
static bool checksum_algo_38(std::string const& account)
{
    static const vector<int> weights = {2, 4, 8, 5, 10, 9};

    return check_checksum(account, sum_add_mod(account, weights, 11, 3, 8), 9);
}

// 39
// not assigned to any bank
static bool checksum_algo_39(std::string const& account)
{
    static const vector<int> weights = {2, 4, 8, 5, 10, 9, 7};

    return check_checksum(account, sum_add_mod(account, weights, 11, 2, 8), 9);
}

// 40
static bool checksum_algo_40(std::string const& account)
{
    static const vector<int> weights = {2, 4, 8, 5, 10, 9, 7, 3, 6};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 41
static bool checksum_algo_41(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    return check_checksum(account, sum_add_mod(account, weights, 11, (account[3] != '9') ? 0 : 3, 8), 9);
}

// 42
static bool checksum_algo_42(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9};

    return check_checksum(account, sum_add_mod(account, weights, 11, 1, 8), 9);
}

// 43
static bool checksum_algo_43(std::string const& account)
{
    static const vector<int> weights = {1, 2, 3, 4, 5, 6, 7, 8, 9};

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 44
static bool checksum_algo_44(std::string const& account)
{
    static const vector<int> weights = {2, 4, 8, 5, 10};

    return check_checksum(account, sum_add_mod(account, weights, 11, 4, 8), 9);
}

// 45
// not assigned to any bank
static bool checksum_algo_45(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (account[0] == '0' || account[4] == '1' || (account[0] == '4' && account[1] == '8'))
    {
        return true;
    }

    return check_checksum(account, sum_add_mod(account, weights, 10));
}

// 46
static bool checksum_algo_46(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6};

    return check_checksum(account, sum_add_mod(account, weights, 11, 2, 6), 7);
}

// 47
static bool checksum_algo_47(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6};

    return check_checksum(account, sum_add_mod(account, weights, 11, 3, 7), 8);
}

// 48
static bool checksum_algo_48(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    return check_checksum(account, sum_add_mod(account, weights, 11, 2, 7), 8);
}

// 49
static bool checksum_algo_49(std::string const& account)
{
    return checksum_algo_00(account) || checksum_algo_01(account);
}

// 50
static bool checksum_algo_50(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    if (check_checksum(account, sum_add_mod(account, weights, 11, 0, 5), 6))
    {
        return true;
    }

    // this part is only a suggestion in pruefzifferberechnungsmethoden-data.pdf
    auto imaginary_account = account.substr(3, 4) + "000";

    return check_checksum(account, sum_add_mod(account, weights, 11, 0, 5), 6);
}

// 51
// 56
// 57
// 59
// 60
// 61
// 63
// 64
// 65
// 66
// 67
// 68
// 71
// 73
// 74
// 76
// 78
// 81
// 85
// 87
// 88
// 91
// 92
// 94
// 95
// 96
// 98
// 99
// A2
// A3
// A4
// A5
// A6
// A7
// A8
// B1
// B2
// B3
// B5
// B6
// B7
// B8
// C0
// C1
// C2
// C3
// C5
// C7
// C8
// C9
// D0
// D2
// D6
// D7
// D8
// D9
// E0
// E3
// E4

static const std::map<std::string, std::function<bool(std::string const&)>> checksum_algorithms = {
{"00", checksum_algo_00},
{"01", checksum_algo_01},
{"02", checksum_algo_02},
{"03", checksum_algo_03},
{"04", checksum_algo_04},
{"05", checksum_algo_05},
{"06", checksum_algo_06},
{"07", checksum_algo_07},
{"08", checksum_algo_08},
{"09", checksum_algo_09},
{"10", checksum_algo_09},
{"11", checksum_algo_09},
{"12", checksum_algo_09},
{"13", checksum_algo_09},
{"14", checksum_algo_09},
{"15", checksum_algo_09},
{"16", checksum_algo_09},
{"17", checksum_algo_09},
{"18", checksum_algo_09},
{"19", checksum_algo_09},
{"20", checksum_algo_09},
{"21", checksum_algo_09},
{"22", checksum_algo_09},
{"23", checksum_algo_09},
{"24", checksum_algo_09},
{"25", checksum_algo_09},
{"26", checksum_algo_09},
{"27", checksum_algo_09},
{"28", checksum_algo_09},
{"29", checksum_algo_09},
{"30", checksum_algo_09},
{"31", checksum_algo_09},
{"32", checksum_algo_09},
{"33", checksum_algo_09},
{"34", checksum_algo_09},
{"35", checksum_algo_09},
{"36", checksum_algo_09},
{"37", checksum_algo_09},
{"38", checksum_algo_09},
{"39", checksum_algo_09},
{"40", checksum_algo_09},
{"41", checksum_algo_09},
{"42", checksum_algo_09},
{"43", checksum_algo_09},
{"44", checksum_algo_09},
{"45", checksum_algo_09},
{"46", checksum_algo_09},
{"47", checksum_algo_09},
{"48", checksum_algo_09},
{"49", checksum_algo_09},
{"50", checksum_algo_09},
};

bool BBan_handler_DE::is_valid_checksum(std::string const& bban) const
{
    auto bankcode = bban.substr(m_iban_structure.bank_code.first, m_iban_structure.bank_code.second - m_iban_structure.bank_code.first);

    auto bank = Bic_repository::get_instance()->get_by_country_bankcode(m_country, bankcode);
    if (bank.empty())
    {
        // bank unknown, cannot determine checksum algorithm
        return false;
    }

    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("DE algo: " << bank[0].get().checksum_algo.value_or("*BLZ not found*")));

    auto it = checksum_algorithms.find(bank[0].get().checksum_algo.value_or(""));
    if (it == checksum_algorithms.end())
    {
        // checksum algorithm not implemented
        // return false;
        throw Iban_error("DE algorithm unknown: " + bank[0].get().checksum_algo.value_or(""));
    }

    return it->second(bban.substr(8, 10));
}

__attribute__((weak)) bool BBan_handler_DE::is_valid_ext(std::string const& bban) const
{
    return true;
}

std::string BBan_handler_DE::preformat(std::string const& bban) const
{
    const regex trim("\\s");

    const regex numeric("^.*([0-9]{18}).*$");
    const regex with_hyphen("^.*([0-9]{8})-([0-9]{1,10}).*$");
    const regex with_blz("^.*([0-9]{1,10})BLZ([0-9]{8}).*$");
    const string padding("0000000000");

    auto trimmed_bban = regex_replace(bban, trim, "");

    // 532013000 BLZ 37040044
    smatch blz_result;
    regex_match(trimmed_bban, blz_result, with_blz);

    if (blz_result.size() == 3)
    {
        auto blz = blz_result[2].str();
        auto account = blz_result[1].str();

        account = padding.substr(0, 10 - account.size()) + account;

        return blz + account;
    }

    // 37040044-532013000
    smatch hyphen_result;
    regex_match(trimmed_bban, hyphen_result, with_hyphen);

    if (hyphen_result.size() == 3)
    {
        auto blz = hyphen_result[1].str();
        auto account = hyphen_result[2].str();

        account = padding.substr(0, 10 - account.size()) + account;

        return blz + account;
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

std::string BBan_handler_DE::trim(std::string const& bban) const
{
    return bban;
}

std::string BBan_handler_DE::format(std::string const& bban) const
{
    auto blz = bban.substr(0, 8);
    auto account = bban.substr(8);

    account.erase(0, min(account.find_first_not_of('0'), account.size()));

    auto result = account + " BLZ " + blz;

    return result;
}

} // namespace countries
} // namespace iban
