/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 * 
 * Algorithm description:
 *   - https://www.bundesbank.de/de/aufgaben/unbarer-zahlungsverkehr/serviceangebot/pruefzifferberechnung/pruefzifferberechnung-fuer-kontonummern-603282
 * 
 * Code ideas:
 *   - https://github.com/mdomke/schwifty/schwifty/checksum/germany.py
 *   - https://github.com/globalcitizen/php-iban/blob/master/php-iban.php
 * 
 * Cross validation:
 *   - https://www.ibancalculator.com/bic_und_iban.html
 */

#include "iban/countries/DE.h"
#include "iban/bic.h"
#include "iban/common.h"
#include "iban/error.h"

// #include <log4cplus/logger.h>
// #include <log4cplus/loggingmacros.h>
#include <regex>
#include <vector>

using std::map;
using std::min;
using std::regex;
using std::regex_match;
using std::regex_replace;
using std::set;
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
static int sum_add_mod_master(std::string const& account, std::vector<int> const& weights, int modulus,
                              bool sum_master_num = false, size_t first_digit = 0, size_t last_digit = 8, int sum_shift = 0)
{
    size_t mod = weights.size();
    int sum = 0;
    size_t i = last_digit + 1;
    size_t p = 0;

    do
    {
        --i;
        // master number is the sum of the digits of the weighted digit
        auto digit_weight = (account[i] - '0') * weights[p];
        sum += (digit_weight < 10) ? digit_weight : (digit_weight / 10 + digit_weight % 10);

        ++p;
        p %= weights.size();
    } while (i > first_digit);

    sum += sum_shift;

    if (sum_master_num)
    {
        while (sum > 9)
        {
            sum = sum / 10 + sum % 10;
        }
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
        sum += mapping[(8 - p) % 4][account[p] - '0'];
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

    return check_checksum(account, sum_add_mod_master(account, weights, 10));
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

    auto check = sum_add_mod(account, weights, 11) % 10;

    return check_checksum(account, check);
}

// 07
static bool checksum_algo_07(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 08
static bool checksum_algo_08(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    uint64_t account_numeric = stoll(account);

    if (account_numeric >= 60000LL)
    {
        return check_checksum(account, sum_add_mod_master(account, weights, 10));
    }
    else
    {
        return true;
    }

    //    return check_checksum(account, sum_add_mod(account, weights, 10));
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

    return check_checksum(account, sum_add_mod(account, weights, 11) % 10);
}

// 11
static bool checksum_algo_11(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto check = sum_add_mod(account, weights, 11);

    if (check == 10)
    {
        check = 9;
    }

    return check_checksum(account, check);
}

// 12
// free - not assigned to any bank
static bool checksum_algo_12(std::string const& account)
{
    return false;
}

// 13
static bool checksum_algo_13(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (check_checksum(account, sum_add_mod_master(account, weights, 10, false, 1, 6), 7))
    {
        return true;
    }

    auto modified_account = account.substr(2) + "00";

    return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 1, 6), 7);
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

    if (check == 10)
    {
        return (account[8] == account[9]);
    }

    return check_checksum(account, check);
}

// 17
static bool checksum_algo_17(std::string const& account)
{
    // reverse order, definition writes left to right
    static const vector<int> weights = {2, 1, 2, 1, 2, 1};

    auto check = (11 - sum_add_mod_master(account, weights, 11, false, 1, 6, -1)) % 11;

    // doc saying the we have to subtract the remainder from 10, not from modulus (=11)
    check = (10 - check) % 10;

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

    return check_checksum(account, sum_add_mod(account, weights, 11) % 10);
}

// 20
static bool checksum_algo_20(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8, 9, 3};

    return check_checksum(account, sum_add_mod(account, weights, 11) % 10);
}

// 21
static bool checksum_algo_21(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    auto check = sum_add_mod_master(account, weights, 10, true);

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
        sum += ((short_account[i] - '0' + 1) * weights[i % weights.size()]) % 11;
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

    auto shifted_account = (account[0] == '0' && account[1] == '0') ? account.substr(2) + "00" : account;
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

        check = sum_add_mod_master(account, weights, 10);
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

    auto check = sum_add_mod(account, weights, 11, 0, 6) % 10;

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

    auto check = (11 - sum_add_mod(account, weights, 11)) % 11;

    if (check == 10)
    {
        return false;
    }

    return check_checksum(account, check);
}

// 32
static bool checksum_algo_32(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    return check_checksum(account, sum_add_mod(account, weights, 11, 3, 8) % 10, 9);
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

    auto check = sum_add_mod(account, weights, 11, 0, 6) % 10;

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

    auto check = sum_add_mod(account, weights, 11, 3, 8) % 10;

    return check_checksum(account, check, 9);
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

    auto check = sum_add_mod(account, weights, 11) % 10;

    return check_checksum(account, check);
}

// 41
static bool checksum_algo_41(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    return check_checksum(account, sum_add_mod_master(account, weights, 10, false, (account[3] != '9') ? 0 : 3, 8), 9);
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

    return check_checksum(account, sum_add_mod_master(account, weights, 10));
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
static bool checksum_algo_51(std::string const& account)
{
    static const vector<int> weights_A = {2, 3, 4, 5, 6, 7};
    static const vector<int> weights_B = {2, 3, 4, 5, 6};
    static const vector<int> weights_C = {2, 1};
    static const vector<int> weights_E1 = {2, 3, 4, 5, 6, 7, 8};
    static const vector<int> weights_E2 = {2, 3, 4, 5, 6, 7, 8, 9, 10};

    if (account[2] == '9')
    {

        return (check_checksum(account, sum_add_mod(account, weights_E1, 11, 2, 8), 9)      // exception 1
                || check_checksum(account, sum_add_mod(account, weights_E2, 11, 0, 8), 9)); // exception 2
    }

    return (check_checksum(account, sum_add_mod(account, weights_A, 11, 3, 8), 9)                  // methode A
            || check_checksum(account, sum_add_mod(account, weights_B, 11, 4, 8), 9)               // methode B
            || check_checksum(account, sum_add_mod_master(account, weights_C, 10, false, 3, 8), 9) // methode C
            || check_checksum(account, sum_add_mod(account, weights_B, 7, 4, 8), 9));              // methode D
}

// 52
// not assigned to any bank
static bool checksum_algo_52(std::string const& account, std::string const& blz)
{
    static const vector<int> weights = {2, 4, 8, 5, 10, 9, 7, 3, 6, 1, 2, 4};

    if (account[0] == '9')
    {
        return checksum_algo_20(account);
    }

    auto short_account = account;
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));

    auto very_short_account = short_account.substr(2);
    very_short_account.erase(0, min(very_short_account.find_first_not_of('0'), very_short_account.size()));

    string alt_account = blz.substr(4, 4) + short_account.substr(0, 1) + "0" + very_short_account;

    auto check = (11 - sum_add_mod(alt_account, weights, 11, 0, alt_account.size() - 1)) % 11;
    auto multiplier = weights[very_short_account.size()];
    auto i = -1;
    auto m = 0;
    do
    {
        ++i;
        m = check + i * multiplier;
    } while ((m % 11) != 10);

    return check_checksum(short_account, i, 1);
}

// 53
// not assigned to any bank
static bool checksum_algo_53(std::string const& account, std::string const& blz)
{
    static const vector<int> weights = {2, 4, 8, 5, 10, 9, 7, 3, 6, 1, 2, 4};

    if (account[0] == '9')
    {
        return checksum_algo_20(account);
    }

    // checksum is the number one should write into 'P' in order
    // to have 10 as a remainder of the weighted summary mod 11
    // operation

    string alt_account = blz.substr(4, 2) + account.substr(2, 1) + blz.substr(7, 1) + account.substr(1, 1) + "0" + account.substr(4);

    auto check = (11 - sum_add_mod(alt_account, weights, 11, 0, 11)) % 11;
    auto multiplier = weights[6];
    auto i = -1;
    auto m = 0;
    do
    {
        ++i;
        m = check + i * multiplier;
    } while ((m % 11) != 10);

    return check_checksum(account, i, 3);
}

// 54
// not assigned to any bank
static bool checksum_algo_54(std::string const& account)
{
    static const vector<int> weights_A = {2, 3, 4, 5, 6, 7};

    if (account[0] != '4' || account[1] != '9')
    {
        return false;
    }

    return check_checksum(account, sum_add_mod(account, weights_A, 11, 2, 8), 9);
}

// 55
// not assigned to any bank
static bool checksum_algo_55(std::string const& account)
{
    static const vector<int> weights_A = {2, 3, 4, 5, 6, 7, 8, 7, 8};

    return check_checksum(account, sum_add_mod(account, weights_A, 11));
}

// 56
static bool checksum_algo_56(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    if (account[0] != '9')
    {
        return check_checksum(account, sum_add_mod(account, weights, 11));
    }

    auto check = sum_add_mod(account, weights, 11);
    switch (check)
    {
        case 10:
            check = 7;
            break;

        case 0:
            check = 8;
            break;

        default:
            break;
    }

    return check_checksum(account, check);
}

// 57
static bool checksum_algo_57(std::string const& account)
{
    static const vector<int> weights = {1, 2};
    // variant 1 {51, 55, 61, 64, 65, 66, 70, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 88, 94, 95}
    // variant 2 {32, 33, 34, 35, 36, 37, 38, 39, 41, 42, 43, 44, 45, 46, 47, 48, 49, 52, 53, 54, 56, 57, 58, 59, 60, 62, 63, 67, 68, 69, 71, 72, 83, 84, 85, 86, 87, 89, 90, 92, 93, 96, 97, 98}
    // variant 3 {40, 50, 91, 99}
    // variant 4 {01, 02, 03, 04, 05, 06, 07, 08, 09, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}
    static const map<int, int> variants = {{1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4}, {7, 4}, {8, 4}, {9, 4}, {10, 4}, {11, 4}, {12, 4}, {13, 4}, {14, 4}, {15, 4}, {16, 4}, {17, 4}, {18, 4}, {19, 4}, {20, 4}, {21, 4}, {22, 4}, {23, 4}, {24, 4}, {25, 4}, {26, 4}, {27, 4}, {28, 4}, {29, 4}, {30, 4}, {31, 4}, {32, 2}, {33, 2}, {34, 2}, {35, 2}, {36, 2}, {37, 2}, {38, 2}, {39, 2}, {40, 3}, {41, 2}, {42, 2}, {43, 2}, {44, 2}, {45, 2}, {46, 2}, {47, 2}, {48, 2}, {49, 2}, {50, 3}, {51, 1}, {52, 2}, {53, 2}, {54, 2}, {55, 1}, {56, 2}, {57, 2}, {58, 2}, {59, 2}, {60, 2}, {61, 1}, {62, 2}, {63, 2}, {64, 1}, {65, 1}, {66, 1}, {67, 2}, {68, 2}, {69, 2}, {70, 1}, {71, 2}, {72, 2}, {73, 1}, {74, 1}, {75, 1}, {76, 1}, {77, 1}, {78, 1}, {79, 1}, {80, 1}, {81, 1}, {82, 1}, {83, 2}, {84, 2}, {85, 2}, {86, 2}, {87, 2}, {88, 1}, {89, 2}, {90, 2}, {91, 3}, {92, 2}, {93, 2}, {94, 1}, {95, 1}, {96, 2}, {97, 2}, {98, 2}, {99, 3}};

    if (account.substr(0, 6) == "777777" || account.substr(0, 6) == "888888" || account == "0185125434")
    {
        // these are exceptions
        return true;
    }

    int id = stoi(account.substr(0, 2));
    if (variants.find(id) == variants.end())
    {
        return false;
    }

    switch (variants.at(id))
    {
        case 1:
            return check_checksum(account, sum_add_mod_master(account, weights, 10));

        case 2:
        {
            // check digit is the 3rd
            auto modified_account = account.substr(0, 2) + account.substr(3);
            return check_checksum(account, sum_add_mod_master(modified_account, weights, 10), 2);
        }

        case 3:
            // no checksum at all
            return true;

        case 4:
        {
            // no checksum calculation, only some value checks
            int id_2 = stoi(account.substr(2, 2));
            int id_3 = stoi(account.substr(6, 3));

            if (id_2 < 1 || id_2 > 12 || id_3 >= 500)
            {
                return false;
            }

            return true;
        }
        break;

        default:
            break;
    }

    return false;
}

// 58
// not assigned to any bank
static bool checksum_algo_58(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 0, 0, 0, 0};

    return check_checksum(account, sum_add_mod(account, weights, 11));
}

// 59
static bool checksum_algo_59(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    auto digits = account.size() - min(account.find_first_not_of('0'), account.size());

    if (digits < 9)
    {
        // no check if length is less than 9 digits
        return true;
    }

    return check_checksum(account, sum_add_mod_master(account, weights, 10));
}

// 60
static bool checksum_algo_60(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 2, 8), 9);
}

// 61
static bool checksum_algo_61(std::string const& account)
{
    static const vector<int> weights = {2, 1, 2, 1, 2, 1, 2, 0, 1, 2};

    auto check = sum_add_mod_master(account, weights, 10, false, 0, (account[8] == '8') ? 6 : 9, 0);

    return check_checksum(account, check, 7);
}

// 62
// not assigned to any bank
static bool checksum_algo_62(std::string const& account)
{
    static const vector<int> weights = {2, 1, 2, 1, 2};

    auto check = sum_add_mod_master(account, weights, 10, false, 2, 6, 0);

    return check_checksum(account, check, 7);
}

// 63
static bool checksum_algo_63(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (account[0] != '0')
    {
        return false;
    }

    if (account.substr(0, 3) != "000")
    {
        auto check = sum_add_mod_master(account, weights, 10, false, 1, 6, 0);
        return check_checksum(account, check, 7);
    }
    else
    {
        auto check = sum_add_mod_master(account, weights, 10, false, 3, 8, 0);
        return check_checksum(account, check, 9);
    }
}

// 64
static bool checksum_algo_64(std::string const& account)
{
    static const vector<int> weights = {2, 4, 8, 5, 10, 9};

    return check_checksum(account, sum_add_mod(account, weights, 11, 0, 5), 6);
}

// 65
static bool checksum_algo_65(std::string const& account)
{
    static const vector<int> weights = {2, 1};
    static const vector<int> weights_exception = {2, 1, 0, 2, 1, 2, 1, 2, 1, 2};

    auto check = 0;
    if (account[8] != '9')
    {
        check = sum_add_mod_master(account, weights, 10, false, 0, 6, 0);
    }
    else
    {
        check = sum_add_mod_master(account, weights_exception, 10, false, 0, 9, 0);
    }

    return check_checksum(account, check, 7);
}

// 66
static bool checksum_algo_66(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 0, 0, 7, 0};

    if (account[0] != '0')
    {
        return false;
    }

    if (account[1] != '9')
    {
        return true;
    }

    auto check = sum_add_mod_master(account, weights, 11);

    switch (check)
    {
        case 10:
            check = 0;
            break;

        case 0:
            check = 1;
            break;

        default:
            break;
    }

    return check_checksum(account, check, 7);
}

// 67
static bool checksum_algo_67(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 0, 6), 7);
}

// 68
static bool checksum_algo_68(std::string const& account)
{
    static const vector<int> weights = {2, 1};
    static const vector<int> weights_partial = {2, 1, 2, 1, 2, 0, 0, 1};

    auto short_account_length = account.size() - min(account.find_first_not_of('0'), account.size());

    auto check = 0;
    switch (short_account_length)
    {
        case 10:
            if (account[3] != '9')
            {
                return false;
            }

            check = sum_add_mod_master(account, weights, 10, false, 3, 8, 0);
            return check_checksum(account, check, 9);

        case 9:
            if (account[1] == '4')
            {
                return true;
            }
            // no break here

        case 8:
        case 7:
        case 6:
            return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 1, 8, 0), 9)
                   || check_checksum(account, sum_add_mod_master(account, weights_partial, 10, false, 1, 8, 0), 9);

        default:
            break;
    }

    return false;
}

// 69
// not assigned to any bank
static bool checksum_algo_69(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8};

    auto id = stoi(account.substr(0, 2));

    switch (id)
    {
        case 93:
            // no checksum
            return true;

        case 97:
            // only variant 2 possible
            return check_checksum(account, sum_mapper(account));

        default:
        {
            // variant 1
            auto check = sum_add_mod(account, weights, 11, 0, 6) % 10;

            if (check_checksum(account, check, 7))
            {
                return true;
            }

            // variant 2
            return check_checksum(account, sum_mapper(account));
        }
        break;
    }
}

// 70
// not assigned to any bank
static bool checksum_algo_70(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    if (account[3] == '5' || (account[3] == '6' && account[4] == '9'))
    {
        return check_checksum(account, sum_add_mod(account, weights, 11, 3, 8), 9);
    }

    return check_checksum(account, sum_add_mod(account, weights, 11, 0, 8), 9);
}

// 71
static bool checksum_algo_71(std::string const& account)
{
    static const vector<int> weights = {1, 2, 3, 4, 5, 6};

    return check_checksum(account, sum_add_mod(account, weights, 11, 1, 6), 9);
}

// 72
// not assigned to any bank
static bool checksum_algo_72(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    return check_checksum(account, sum_add_mod_master(account, weights, 11, false, 3, 8), 9);
}

// 73
static bool checksum_algo_73(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (account[2] == '9')
    {
        return checksum_algo_51(account);
    }

    return (check_checksum(account, sum_add_mod_master(account, weights, 10, false, 3, 8), 9)
            || check_checksum(account, sum_add_mod_master(account, weights, 10, false, 4, 8), 9)
            || check_checksum(account, sum_add_mod_master(account, weights, 7, false, 4, 8), 9));
}

// 74
static bool checksum_algo_74(std::string const& account)
{
    static const vector<int> weights = {2, 1, 2, 1, 2, 1, 2, 1, 2};

    auto short_account = account;
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));

    bool result = checksum_algo_00(account);

    // 1st variant
    if (!result && short_account.size() == 6)
    {
        auto check_a = sum_add_mod_master(account, weights, 10, false, 4, 8, 0);

        auto remainder = 10 - check_a;
        auto check_b = 5 - (remainder % 5); // rounding by the rules
        result = check_checksum(account, check_a) || check_checksum(account, check_b);
    }

    return result || checksum_algo_04(account);
}

// 75
// not assigned to any bank
static bool checksum_algo_75(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (account[0] != '0')
    {
        return false;
    }

    auto short_account = account;
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));

    if (short_account.size() <= 7)
    {
        return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 4, 8), 9);
    }
    else
    {
        if (account[1] == '9')
        {
            return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 1, 6), 7);
        }
        else
        {
            return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 1, 5), 6);
        }
    }
}

// 76
static bool checksum_algo_76(std::string const& account)
{
    // description is ambiguous on whether 6 is in the weight list or not
    static const vector<int> weights = {2, 3, 4, 5, 6};
    static const set<int> types = {'0', '4', '6', '7', '8', '9'};

    if (types.find(account[0]) == types.end())
    {
        return false;
    }

    return check_checksum(account, 11 - sum_add_mod(account, weights, 11, 1, 6), 7)
           || check_checksum(account, sum_add_mod(account, weights, 11));
}

// 77
// not assigned to any bank
static bool checksum_algo_77(std::string const& account)
{
    static const vector<int> weights_1 = {1, 2, 3, 4, 5};
    static const vector<int> weights_2 = {5, 4, 3, 4, 5};
    auto check = sum_add_mod(account, weights_1, 11);
    if (check == 10)
    {
        check = sum_add_mod(account, weights_2, 11);
    }

    return check_checksum(account, check);
}

// 78
static bool checksum_algo_78(std::string const& account)
{
    auto short_account = account;
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));

    if (short_account.size() != 8)
    {
        return checksum_algo_00(account);
    }
    else
    {
        return true;
    }
}

// 79
// not assigned to any bank
static bool checksum_algo_79(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (account[0] == '0')
    {
        return false;
    }

    if (!(account[0] == '1' || account[0] == '2' || account[0] == '9'))
    {
        return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 0, 8), 9);
    }
    else
    {
        return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 0, 7), 8);
    }
}

// 80
// not assigned to any bank
static bool checksum_algo_80(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (account[2] == '9')
    {
        return checksum_algo_51(account);
    }

    // Methode B description is ambiguous (similar to methode A vs. use of master numbers)

    return check_checksum(account, sum_add_mod_master(account, weights, 10, false, 4, 8), 9)
           || check_checksum(account, sum_add_mod_master(account, weights, 7, false, 4, 8), 9);
}

// 81
static bool checksum_algo_81(std::string const& account)
{
    if (account[2] != '9')
    {
        return checksum_algo_32(account);
    }
    else
    {
        return checksum_algo_51(account);
    }
}

// 82
// not assigned to any bank
static bool checksum_algo_82(std::string const& account)
{
    if (account[2] == '9' && account[3] == '9')
    {
        return checksum_algo_33(account);
    }
    else
    {
        return checksum_algo_10(account);
    }
}

// 83
// not assigned to any bank
static bool checksum_algo_83(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8};

    if (account[2] == '9' && account[3] == '9')
    {
        return check_checksum(account, sum_add_mod(account, weights, 11, 2, 8), 9);
    }
    else
    {
        return checksum_algo_32(account)
               || checksum_algo_33(account)
               || check_checksum(account, sum_add_mod(account, weights, 7, 4, 8), 9);
    }
}

// 84
// not assigned to any bank
static bool checksum_algo_84(std::string const& account)
{
    static const vector<int> weights_ab = {2, 3, 4, 5, 6};
    static const vector<int> weights_c = {2, 1};

    if (account[2] == '9')
    {
        return checksum_algo_51(account);
    }
    else
    {
        return check_checksum(account, sum_add_mod(account, weights_ab, 11, 4, 8), 9)
               || check_checksum(account, sum_add_mod(account, weights_ab, 7, 4, 8), 9)
               || check_checksum(account, sum_add_mod(account, weights_c, 10, 4, 8), 9);
    }
}

// 85
static bool checksum_algo_85(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8};

    if (account[2] == '9' && account[3] == '9')
    {
        return check_checksum(account, sum_add_mod(account, weights, 11, 2, 8), 9);
    }
    else
    {
        return check_checksum(account, sum_add_mod(account, weights, 11, 3, 8), 9)
               || check_checksum(account, sum_add_mod(account, weights, 11, 4, 8), 9)
               || check_checksum(account, sum_add_mod(account, weights, 7, 4, 8), 9);
    }
}

// 86
// not assigned to any bank
static bool checksum_algo_86(std::string const& account)
{
    static const vector<int> weights_a = {2, 1};
    static const vector<int> weights_b = {2, 3, 4, 5, 6, 7};

    if (account[2] == '9')
    {
        return checksum_algo_51(account);
    }
    else
    {
        return check_checksum(account, sum_add_mod_master(account, weights_a, 10, false, 3, 8), 9)
               || check_checksum(account, sum_add_mod(account, weights_b, 11, 3, 8), 9);
    }
}

enum class state_87
{
    A,
    B,
    C,
    D
};

// helper for 87
static bool checksum_algo_87_a(std::string const& account)
{
    static const vector<int> tab_1 = {0, 4, 3, 2, 6};
    static const vector<int> tab_2 = {7, 1, 5, 9, 8};
    static const vector<int> konto_digits = {5, 6, 2, 3, 4, 10, 1, 7, 8, 9}; // to replace some of the values

    size_t i = 3;
    while (account[i] == '0' && i < 9)
    {
        ++i;
    }

    vector<int> konto;
    for (const char c : account)
    {
        konto.push_back(c - '0');
    }

    state_87 st = (i % 2) ? state_87::A : state_87::C;

    int a5 = 0;
    do
    {
        konto[i] = konto_digits[konto[i]];

        switch (st)
        {
            case state_87::A:
                st = (konto[i] > 5) ? state_87::D : state_87::C;
                a5 += (konto[i] > 5) ? 12 - konto[i] : konto[i];
                break;

            case state_87::B:
                st = (konto[i] > 5) ? state_87::C : state_87::D;
                a5 += (konto[i] > 5) ? -12 + konto[i] : -konto[i];
                break;

            case state_87::C:
                st = (konto[i] > 5) ? state_87::B : state_87::A;
                a5 += -konto[i];
                break;

            case state_87::D:
                st = (konto[i] > 5) ? state_87::A : state_87::B;
                a5 += konto[i];
                break;
        }
    } while (++i < 9);

    if (a5 < 0)
    {
        a5 -= (a5 / 5 - 1) * 5;
    }

    int p = (st == state_87::A || st == state_87::C) ? tab_1[a5 % 5] : tab_2[a5 % 5];

    if (p == (account[9] - '0'))
    {
        return true;
    }
    else
    {
        if (account[3] - '0' == 0)
        {
            p += (p > 4) ? -5 : 5;
        }

        if (p == account[9] - '0')
        {
            return true;
        }
    }

    return false;
}


// 87
static bool checksum_algo_87(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    if (account[2] == '9')
    {
        return checksum_algo_51(account);
    }
    else
    {
        return checksum_algo_87_a(account)
               || check_checksum(account, sum_add_mod(account, weights, 11, 4, 8) % 10, 9)
               || check_checksum(account, sum_add_mod(account, weights, 7, 4, 8), 9)
               || check_checksum(account, sum_add_mod(account, weights, 11, 3, 8) % 10, 9);
    }
}

// 88
static bool checksum_algo_88(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8};

    return check_checksum(account, sum_add_mod(account, weights, 11, (account[2] != '9') ? 3 : 2, 8) % 10, 9);
}

// 89
// not assigned to any bank
static bool checksum_algo_89(std::string const& account)
{
    static const vector<int> weights_a = {2, 1};
    static const vector<int> weights_b = {2, 3, 4, 5, 6, 7};

    auto short_account = account;
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));

    switch (short_account.size())
    {
        case 8:
        case 9:
            return checksum_algo_10(account);

        case 7:
        {
            static const vector<int> weights = {7, 6, 5, 4, 3, 2};

            auto check = sum_add_mod_master(account, weights, 11, false, 3, 8, 0);

            return check_checksum(account, check, 9);
        }
        break;

        default: // 1..6 & 10
            return true;
    }
}

// 90
// not assigned to any bank
static bool checksum_algo_90(std::string const& account)
{
    static const vector<int> weights_abcdf = {2, 3, 4, 5, 6, 7, 8};
    static const vector<int> weights_eg = {2, 1};

    if (account[2] == '9')
    {
        return check_checksum(account, sum_add_mod(account, weights_abcdf, 11, 2, 8), 9); // method F
    }
    else
    {
        return check_checksum(account, sum_add_mod(account, weights_abcdf, 11, 3, 8), 9)    // method A
               || check_checksum(account, sum_add_mod(account, weights_abcdf, 11, 4, 8), 9) // method B
               || check_checksum(account, sum_add_mod(account, weights_abcdf, 7, 4, 8), 9)  // method C
               || check_checksum(account, sum_add_mod(account, weights_abcdf, 9, 4, 8), 9)  // method D
               || check_checksum(account, sum_add_mod(account, weights_eg, 10, 4, 8), 9)    // method E
               || check_checksum(account, sum_add_mod(account, weights_eg, 7, 3, 8), 9);    // method G
    }
}

// 91
static bool checksum_algo_91(std::string const& account)
{
    static const vector<int> weights_1 = {2, 3, 4, 5, 6, 7};
    static const vector<int> weights_2 = {7, 6, 5, 4, 3, 2};
    static const vector<int> weights_3 = {2, 3, 4, 0, 5, 6, 7, 8, 9, 10};
    static const vector<int> weights_4 = {2, 4, 8, 5, 10, 9};

    return check_checksum(account, sum_add_mod(account, weights_1, 11, 0, 5) % 10, 6)     // method 1
           || check_checksum(account, sum_add_mod(account, weights_2, 11, 0, 5) % 10, 6)  // method 2
           || check_checksum(account, sum_add_mod(account, weights_3, 11, 0, 9) % 10, 6)  // method 3
           || check_checksum(account, sum_add_mod(account, weights_4, 11, 0, 5) % 10, 6); // method 4
}

// 92
static bool checksum_algo_92(std::string const& account)
{
    static const vector<int> weights = {3, 7, 1};

    return check_checksum(account, sum_add_mod(account, weights, 10, 3, 8), 9);
}

// 93
// not assigned to any bank, but used by A4
static bool checksum_algo_93(std::string const& account)
{
    static const vector<int> weights_abcdf = {2, 3, 4, 5, 6};

    if (account.substr(0, 4) != "0000")
    {
        // A type
        return check_checksum(account, sum_add_mod(account, weights_abcdf, 11, 0, 4) % 10, 5)
               || check_checksum(account, sum_add_mod(account, weights_abcdf, 7, 0, 4) % 10, 5);
    }
    else
    {
        // B type
        return check_checksum(account, sum_add_mod(account, weights_abcdf, 11, 4, 8), 9)
               || check_checksum(account, sum_add_mod(account, weights_abcdf, 7, 4, 8), 9);
    }
}

// 94
static bool checksum_algo_94(std::string const& account)
{
    static const vector<int> weights = {1, 2};

    return check_checksum(account, sum_add_mod_master(account, weights, 10));
}

// 95
static bool checksum_algo_95(std::string const& account)
{
    uint64_t account_numeric = stoll(account);

    if ((account_numeric >= 1LL && account_numeric <= 1999999LL)
        || (account_numeric >= 9000000LL && account_numeric <= 25999999LL)
        || (account_numeric >= 396000000LL && account_numeric <= 499999999LL)
        || (account_numeric >= 700000000LL && account_numeric <= 799999999LL)
        || (account_numeric >= 910000000LL && account_numeric <= 989999999LL))
    {
        return true;
    }
    else
    {
        return checksum_algo_06(account);
    }
}

// 96
static bool checksum_algo_96(std::string const& account)
{
    bool result = checksum_algo_19(account) || checksum_algo_00(account);

    if (result)
    {
        return true;
    }

    uint64_t account_numeric = stoll(account);
    return (account_numeric >= 1300000LL && account_numeric <= 99399999LL);
}

// 97
// not assigned to any bank
static bool checksum_algo_97(std::string const& account)
{
    uint64_t account_numeric = stoll(account);

    return (account[9] == (account_numeric % 11) + '0');
}

// 98
static bool checksum_algo_98(std::string const& account)
{
    static const vector<int> weights = {3, 1, 7};

    return check_checksum(account, sum_add_mod(account, weights, 10, 2, 8), 9)
           || checksum_algo_32(account);
}

// 99
static bool checksum_algo_99(std::string const& account)
{
    uint64_t account_numeric = stoll(account);

    if ((account_numeric >= 396000000LL && account_numeric <= 499999999LL))
    {
        return true;
    }
    else
    {
        return checksum_algo_06(account);
    }
}

// A0
// not assigned to any bank
static bool checksum_algo_a0(std::string const& account)
{
    static const vector<int> weights = {2, 4, 8, 5, 10, 0, 0, 0, 0};

    auto check = sum_add_mod(account, weights, 11) % 10;

    return check_checksum(account, check, 9);
}

// A1
// not assigned to any bank
static bool checksum_algo_a1(std::string const& account)
{
    static const vector<int> weights = {2, 1, 2, 1, 2, 1, 2, 0, 0};

    auto short_account = account;
    short_account.erase(0, min(short_account.find_first_not_of('0'), short_account.size()));

    if (short_account.size() != 8 && short_account.size() != 10)
    {
        return false;
    }

    return check_checksum(account, sum_add_mod_master(account, weights, 11), 9);
}

// A2
static bool checksum_algo_a2(std::string const& account)
{
    return checksum_algo_00(account)
           || checksum_algo_04(account);
}

// A3
static bool checksum_algo_a3(std::string const& account)
{
    return checksum_algo_00(account)
           || checksum_algo_10(account);
}

// A4
static bool checksum_algo_a4(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7};

    return check_checksum(account, sum_add_mod(account, weights, 11, 3, 8), 9)    // method 1
           || check_checksum(account, sum_add_mod(account, weights, 7, 3, 8), 9)  // method 2
           || check_checksum(account, sum_add_mod(account, weights, 11, 4, 8), 9) // method 3
           || checksum_algo_93(account);                                          // method 4
}

// A5
static bool checksum_algo_a5(std::string const& account)
{
    return checksum_algo_00(account)
           || checksum_algo_10(account);
}

// A6
static bool checksum_algo_a6(std::string const& account)
{
    if (account[1] == '8')
    {
        return checksum_algo_00(account);
    }
    else
    {
        return checksum_algo_01(account);
    }
}

// A7
static bool checksum_algo_a7(std::string const& account)
{
    return checksum_algo_00(account)
           || checksum_algo_03(account);
}

// A8
static bool checksum_algo_a8(std::string const& account)
{
    static const vector<int> weights_1 = {2, 3, 4, 5, 6, 7};
    static const vector<int> weights_2 = {2, 1};

    if (account[2] == '9')
    {
        return checksum_algo_51(account);
    }

    return check_checksum(account, sum_add_mod(account, weights_1, 11, 3, 8) % 10, 9)
           || check_checksum(account, sum_add_mod_master(account, weights_2, 10, false, 3, 8), 9);
}

// A9
// not assigned to any bank
static bool checksum_algo_a9(std::string const& account)
{
    return checksum_algo_01(account)
           || checksum_algo_06(account);
}

// B0
// not assigned to any bank
static bool checksum_algo_b0(std::string const& account)
{
    if (account[0] == '0' || account[0] == '8')
    {
        return false;
    }

    switch (account[7])
    {
        case '1':
        case '2':
        case '3':
        case '6':
            return true;

        default:
        {
            return checksum_algo_06(account);
        }
    }
}

// B1
static bool checksum_algo_b1(std::string const& account)
{
    return checksum_algo_05(account)
           || checksum_algo_01(account)
           || checksum_algo_00(account);
}

// B2
static bool checksum_algo_b2(std::string const& account)
{
    if (account[0] <= '7')
    {
        return checksum_algo_02(account);
    }
    else
    {
        return checksum_algo_00(account);
    }
}

// B3
static bool checksum_algo_b3(std::string const& account)
{
    if (account[0] != '9')
    {
        return checksum_algo_32(account);
    }
    else
    {
        return checksum_algo_06(account);
    }
}

// B4
// not assigned to any bank
static bool checksum_algo_b4(std::string const& account)
{
    if (account[0] != '9')
    {
        return checksum_algo_02(account);
    }
    else
    {
        return checksum_algo_00(account);
    }
}

// B5
static bool checksum_algo_b5(std::string const& account)
{
    if (account[0] == '8' || account[0] == '9')
    {
        return false;
    }

    return checksum_algo_05(account)
           || checksum_algo_00(account);
}

// B6
static bool checksum_algo_b6(std::string const& account, std::string const& blz)
{
    if (account[0] > '0' || account.substr(0, 4) == "0269")
    {
        return checksum_algo_20(account);
    }
    else
    {
        return checksum_algo_53(account, blz);
    }
}

// B7
static bool checksum_algo_b7(std::string const& account)
{
    uint64_t account_numeric = stoll(account);

    if ((account_numeric >= 1000000LL && account_numeric <= 5999999LL)
        || (account_numeric >= 700000000LL && account_numeric <= 899999999LL))
    {
        return checksum_algo_01(account);
    }
    else
    {
        return checksum_algo_09(account);
    }
}

// B8
static bool checksum_algo_b8(std::string const& account)
{
    int first_3_digits = stoi(account.substr(0, 3));

    if (checksum_algo_20(account)
        || checksum_algo_29(account))
    {
        return true;
    }

    if ((first_3_digits >= 510 && first_3_digits <= 599)
        || (first_3_digits >= 901 && first_3_digits <= 910))
    {
        return true;
    }

    return false;
}

// B9
// not assigned to any bank
static bool checksum_algo_b9(std::string const& account)
{
    static const vector<int> weights_1 = {1, 3, 2, 1, 3, 2, 1};
    static const vector<int> weights_2 = {1, 2, 3, 4, 5, 6};

    switch (account.find_first_not_of('0'))
    {
        case 2:
        {
            auto sum = 0;
            for (size_t i = 2; i < 9; ++i)
            {
                sum += ((account[i] - '0' + 1) * weights_1[i % weights_1.size()]) % 11;
            }

            auto check = sum % 10;

            return check_checksum(account, check) || check_checksum(account, (check + 5) % 10);
        }

        case 3:
        {

            auto check = sum_add_mod(account, weights_2, 11, 3, 8) % 10;

            // if remainder == 1, than we'll also check if check digit == 5, though
            // the algorithm not directly describes this

            return check_checksum(account, check) || check_checksum(account, (check + 5) % 10);
        }

        default:
            return false;
    }
}

// C0
static bool checksum_algo_c0(std::string const& account, std::string const& blz)
{
    if (account.find_first_not_of('0') == 2)
    {
        return checksum_algo_52(account, blz)
               || checksum_algo_20(account);
    }
    else
    {
        return checksum_algo_20(account);
    }
}

// C1
static bool checksum_algo_c1(std::string const& account)
{
    if (account[0] != '5')
    {
        return checksum_algo_17(account);
    }
    else
    {
        static const vector<int> weights = {1, 2, 1, 2, 1, 2, 1, 2, 1};

        // documentation says, that the remainder to be subtracted not from 11, but from 10
        auto check = sum_add_mod_master(account, weights, 11, false, 0, 8, -1) - 1;
        if (check < 0)
        {
            check = 0;
        }

        return check_checksum(account, check, 9);
    }
}

// C2
static bool checksum_algo_c2(std::string const& account)
{
    return checksum_algo_22(account)
           || checksum_algo_00(account)
           || checksum_algo_04(account);
}

// C3
static bool checksum_algo_c3(std::string const& account)
{
    if (account[0] != '9')
    {
        return checksum_algo_00(account);
    }
    else
    {
        return checksum_algo_58(account);
    }
}

// C4
// not assigned to any bank
static bool checksum_algo_c4(std::string const& account)
{
    if (account[0] != '9')
    {
        return checksum_algo_15(account);
    }
    else
    {
        return checksum_algo_58(account);
    }
}

// C5
static bool checksum_algo_c5(std::string const& account)
{
    auto digits = account.size() - min(account.find_first_not_of('0'), account.size());

    switch (digits)
    {
        case 6:
        {
            if (account[4] != 9)
            {
                return checksum_algo_75(account);
            }
        }
        break;

        case 8:
        {
            if (account[2] != 3 || account[2] != 4 || account[2] != 5)
            {
                return true;
            }
        }
        break;

        case 9:
        {
            if (account[1] != 9)
            {
                return checksum_algo_75(account);
            }
        }
        break;

        case 10:
        {
            if (account[0] == '1' || account[0] == '4' || account[0] == '5' || account[0] == '6' || account[0] == '9')
            {
                return checksum_algo_29(account);
            }
            else if (account[0] == '3')
            {
                return checksum_algo_00(account);
            }
            else if ((account[0] == '7' && account[0] == '0') || (account[0] == '8' || account[1] == '5'))
            {
                return true;
            }
        }
        break;
    }

    return false;
}

// C6
// not assigned to any bank
static bool checksum_algo_c6(std::string const& account)
{
    static const vector<int> weights = {2, 1};
    static const vector<string> prefixes = {"4451970", "4451981", "4451992", "4451993", "4344992", "4344990", "4344991", "5499570", "4451994", "5499579"};

    string prefixed_account = prefixes[account[0] - '0'] + account.substr(1, 8);

    auto check = sum_add_mod_master(prefixed_account, weights, 10, false, 0, 14, 0);

    return check_checksum(account, check);
}

// C7
static bool checksum_algo_c7(std::string const& account)
{
    return checksum_algo_63(account)
           || checksum_algo_06(account);
}

// C8
static bool checksum_algo_c8(std::string const& account)
{
    return checksum_algo_00(account)
           || checksum_algo_04(account)
           || checksum_algo_07(account);
}

// C9
static bool checksum_algo_c9(std::string const& account)
{
    return checksum_algo_00(account)
           || checksum_algo_07(account);
}

// D0
static bool checksum_algo_d0(std::string const& account)
{
    if (account[0] == '5' && account[1] == '7')
    {
        return true;
    }
    else
    {
        return checksum_algo_20(account);
    }
}

// D1
// not assigned to any bank
static bool checksum_algo_d1(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (account[0] == '8')
    {
        return false;
    }

    string prefixed_account = "436338" + account.substr(1, 8);

    auto check = sum_add_mod_master(prefixed_account, weights, 10, false, 0, 14, 0);

    return check_checksum(account, check);
}

// D2
static bool checksum_algo_d2(std::string const& account)
{
    return checksum_algo_95(account)
           || checksum_algo_00(account)
           || checksum_algo_68(account);
}

// D3
// not assigned to any bank
static bool checksum_algo_d3(std::string const& account)
{
    return checksum_algo_00(account)
           || checksum_algo_27(account);
}

// D4
// not assigned to any bank
static bool checksum_algo_d4(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (account[0] == '0')
    {
        return false;
    }

    string prefixed_account = "428259" + account.substr(1, 8);

    auto check = sum_add_mod_master(prefixed_account, weights, 10, false, 0, 14, 0);

    return check_checksum(account, check);
}

// D5
// not assigned to any bank
static bool checksum_algo_d5(std::string const& account)
{
    static const vector<int> weights = {2, 3, 4, 5, 6, 7, 8};

    if (account[2] == '9' && account[3] == '9')
    {
        return check_checksum(account, sum_add_mod(account, weights, 11, 2, 8), 9); // method 1
    }
    else
    {
        return checksum_algo_20(account);
    }

    return check_checksum(account, sum_add_mod(account, weights, 11, 3, 8), 9)     // method 2
           || check_checksum(account, sum_add_mod(account, weights, 7, 3, 8), 9)   // method 3
           || check_checksum(account, sum_add_mod(account, weights, 10, 3, 8), 9); // method 4
}

// D6
static bool checksum_algo_d6(std::string const& account)
{
    // 03 and 00 are essentially the same, yet documentation says they run one after the other
    return checksum_algo_07(account)
           || checksum_algo_03(account)
           || checksum_algo_00(account);
}

// D7
static bool checksum_algo_d7(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    auto check = 10 - sum_add_mod_master(account, weights, 10, false, 0, 8, 0);

    return check_checksum(account, check);
}

// D8
static bool checksum_algo_d8(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    int first_3_digits = stoi(account.substr(0, 3));

    if (first_3_digits >= 1 && first_3_digits <= 9)
    {
        return true;
    }

    auto check = sum_add_mod_master(account, weights, 10, false, 0, 8, 0);

    return check_checksum(account, check);
}

// D9
static bool checksum_algo_d9(std::string const& account)
{
    return checksum_algo_00(account)
           || checksum_algo_10(account)
           || checksum_algo_18(account);
}

// E0
static bool checksum_algo_e0(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    auto check = sum_add_mod_master(account, weights, 10, false, 0, 8, 7);

    return check_checksum(account, check);
}

// E1
// not assigned to any bank
static bool checksum_algo_e1(std::string const& account)
{

    static const vector<int> weights = {9, 10, 11, 6, 5, 4, 3, 2, 1};

    auto sum = 0;
    for (size_t i = 0; i < 9; ++i)
    {
        sum += account[i] * weights[i % weights.size()];
    }

    auto check = sum % 11;

    return check_checksum(account, check);
}

// E2
// not assigned to any bank
static bool checksum_algo_e2(std::string const& account)
{
    static const vector<int> weights = {2, 1};

    if (account[0] >= '6')
    {
        return false;
    }

    string prefixed_account = "438320" + account.substr(1, 8);

    auto check = sum_add_mod_master(prefixed_account, weights, 10, false, 0, 14, 0);

    return check_checksum(account, check);
}

// E3
static bool checksum_algo_e3(std::string const& account)
{
    return checksum_algo_00(account)
           || checksum_algo_21(account);
}

// E4
static bool checksum_algo_e4(std::string const& account)
{
    return checksum_algo_02(account)
           || checksum_algo_00(account);
}

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
{"10", checksum_algo_10},
{"11", checksum_algo_11},
{"12", checksum_algo_12},
{"13", checksum_algo_13},
{"14", checksum_algo_14},
{"15", checksum_algo_15},
{"16", checksum_algo_16},
{"17", checksum_algo_17},
{"18", checksum_algo_18},
{"19", checksum_algo_19},
{"20", checksum_algo_20},
{"21", checksum_algo_21},
{"22", checksum_algo_22},
{"23", checksum_algo_23},
{"24", checksum_algo_24},
{"25", checksum_algo_25},
{"26", checksum_algo_26},
{"27", checksum_algo_27},
{"28", checksum_algo_28},
{"29", checksum_algo_29},
{"30", checksum_algo_30},
{"31", checksum_algo_31},
{"32", checksum_algo_32},
{"33", checksum_algo_33},
{"34", checksum_algo_34},
{"35", checksum_algo_35},
{"36", checksum_algo_36},
{"37", checksum_algo_37},
{"38", checksum_algo_38},
{"39", checksum_algo_39},
{"40", checksum_algo_40},
{"41", checksum_algo_41},
{"42", checksum_algo_42},
{"43", checksum_algo_43},
{"44", checksum_algo_44},
{"45", checksum_algo_45},
{"46", checksum_algo_46},
{"47", checksum_algo_47},
{"48", checksum_algo_48},
{"49", checksum_algo_49},
{"50", checksum_algo_50},
{"51", checksum_algo_51},
{"54", checksum_algo_54},
{"55", checksum_algo_55},
{"56", checksum_algo_56},
{"57", checksum_algo_57},
{"58", checksum_algo_58},
{"59", checksum_algo_59},
{"60", checksum_algo_60},
{"61", checksum_algo_61},
{"62", checksum_algo_62},
{"63", checksum_algo_63},
{"64", checksum_algo_64},
{"65", checksum_algo_65},
{"66", checksum_algo_66},
{"67", checksum_algo_67},
{"68", checksum_algo_68},
{"69", checksum_algo_69},
{"70", checksum_algo_70},
{"71", checksum_algo_71},
{"72", checksum_algo_72},
{"73", checksum_algo_73},
{"74", checksum_algo_74},
{"75", checksum_algo_75},
{"76", checksum_algo_76},
{"77", checksum_algo_77},
{"78", checksum_algo_78},
{"79", checksum_algo_79},
{"80", checksum_algo_80},
{"81", checksum_algo_81},
{"82", checksum_algo_82},
{"83", checksum_algo_83},
{"84", checksum_algo_84},
{"85", checksum_algo_85},
{"86", checksum_algo_86},
{"87", checksum_algo_87},
{"88", checksum_algo_88},
{"89", checksum_algo_89},
{"90", checksum_algo_90},
{"91", checksum_algo_91},
{"92", checksum_algo_92},
{"93", checksum_algo_93},
{"94", checksum_algo_94},
{"95", checksum_algo_95},
{"96", checksum_algo_96},
{"97", checksum_algo_97},
{"98", checksum_algo_98},
{"99", checksum_algo_99},
{"A0", checksum_algo_a0},
{"A1", checksum_algo_a1},
{"A2", checksum_algo_a2},
{"A3", checksum_algo_a3},
{"A4", checksum_algo_a4},
{"A5", checksum_algo_a5},
{"A6", checksum_algo_a6},
{"A7", checksum_algo_a7},
{"A8", checksum_algo_a8},
{"A9", checksum_algo_a9},
{"B0", checksum_algo_b0},
{"B1", checksum_algo_b1},
{"B2", checksum_algo_b2},
{"B3", checksum_algo_b3},
{"B4", checksum_algo_b4},
{"B5", checksum_algo_b5},
{"B7", checksum_algo_b7},
{"B8", checksum_algo_b8},
{"B9", checksum_algo_b9},
{"C1", checksum_algo_c1},
{"C2", checksum_algo_c2},
{"C3", checksum_algo_c3},
{"C4", checksum_algo_c4},
{"C5", checksum_algo_c5},
{"C6", checksum_algo_c6},
{"C7", checksum_algo_c7},
{"C8", checksum_algo_c8},
{"C9", checksum_algo_c9},
{"D0", checksum_algo_d0},
{"D1", checksum_algo_d1},
{"D2", checksum_algo_d2},
{"D3", checksum_algo_d3},
{"D4", checksum_algo_d4},
{"D5", checksum_algo_d5},
{"D6", checksum_algo_d6},
{"D7", checksum_algo_d7},
{"D8", checksum_algo_d8},
{"D9", checksum_algo_d9},
{"E0", checksum_algo_e0},
{"E1", checksum_algo_e1},
{"E2", checksum_algo_e2},
{"E3", checksum_algo_e3},
{"E4", checksum_algo_e4}};

static const std::map<std::string, std::function<bool(std::string const&, std::string const&)>> checksum_algorithms_blz = {
{"52", checksum_algo_52},
{"53", checksum_algo_53},
{"B6", checksum_algo_b6},
{"C0", checksum_algo_c0}};

bool BBan_handler_DE::is_valid_checksum(std::string const& bban) const
{
    auto bankcode = bban.substr(m_iban_structure.bank_code.first, m_iban_structure.bank_code.second - m_iban_structure.bank_code.first);

    auto bank = Bic_repository::get_instance()->get_by_country_bankcode(m_country, bankcode);
    if (bank.empty())
    {
        // bank unknown, cannot determine checksum algorithm
        return false;
    }

    // log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
    // LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("DE algo: " << bank[0].get().checksum_algo.value_or("*check algorithm for BLZ not found*")));

    auto it = checksum_algorithms.find(bank[0].get().checksum_algo.value_or(""));
    if (it != checksum_algorithms.end())
    {
        return it->second(bban.substr(8, 10));
    }

    // some algorithms require also the bank code
    auto it_blz = checksum_algorithms_blz.find(bank[0].get().checksum_algo.value_or(""));
    if (it_blz != checksum_algorithms_blz.end())
    {
        return it_blz->second(bban.substr(8, 10), bban.substr(0, 8));
    }

    // checksum algorithm not implemented
    // return false;
    throw Iban_error("DE algorithm unknown: " + bank[0].get().checksum_algo.value_or(""));
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

    auto trimmed_bban = regex_replace(bban, trim, "");

    // 532013000 BLZ 37040044
    smatch blz_result;
    regex_match(trimmed_bban, blz_result, with_blz);

    if (blz_result.size() == 3)
    {
        auto blz = blz_result[2].str();
        auto account = blz_result[1].str();

        account = string(10 - account.size(), '0') + account;

        return blz + account;
    }

    // 37040044-532013000
    smatch hyphen_result;
    regex_match(trimmed_bban, hyphen_result, with_hyphen);

    if (hyphen_result.size() == 3)
    {
        auto blz = hyphen_result[1].str();
        auto account = hyphen_result[2].str();

        account = string(10 - account.size(), '0') + account;

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
