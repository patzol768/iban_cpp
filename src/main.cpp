/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */ 

#include "iban/bban.h"
#include "iban/bic.h"
#include "iban/countries/countries.h"
#include "iban/country.h"
#include "iban/error.h"
#include "iban/loader.h"

#include <iostream>
#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <string>

using namespace iban;
using namespace iban::countries;
using namespace std;

/** Initializes logging
 *
 *  LOG4CPLUS_CONFIG environment variable shall contain the path of the log4cplus log configuration.
 *  If LOG4CPLUS_CONFIG not defined, than defaults to "log.ini"
 */
void init_logger()
{
    // Initialize logging framework
    // See also: https://stackoverflow.com/questions/33753720/how-do-i-create-a-custom-layout-for-log4cplus
    std::string log_config_filename = "log.ini";
    char* log_config_filename_ptr = getenv("LOG4CPLUS_CONFIG");

    if (log_config_filename_ptr)
    {
        log_config_filename = log_config_filename_ptr;
    }

    log4cplus::PropertyConfigurator::doConfigure(log_config_filename.c_str());

    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("main"));
    LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT("Logging initialized from: " << log_config_filename));

    return;
}

/*
namespace iban
{
namespace countries
{

// is_valid_ext() is a weak function, hence one can add an own validation algorithm
bool BBan_handler_HU::is_valid_ext(std::string const& bban) const
{
    std::cout << "my validator" << std::endl;
    return true;
}

} // namespace countries
} // namespace iban
*/

/** main()
 *
 */
int main(const int parc, const char** pars)
{
    (void)parc;
    (void)pars;

    init_logger();

    // auto result = Country_repository::get_instance()->is_country("HU");
    // std::cout << result << std::endl;

    Bic_repository::get_instance()->load(&bic_loader);

    // Bic bic("DE", "10000000", false);

    Iban_structure_repository::get_instance()->load(&iban_structure_loader);

    REGISTER_ALL_HANDERS();

    Iban iban1("HU05 1176 4379 4089 2883 0000 0000", false, true);
    std::cout << "IBAN: " << iban1 << std::endl;

    Iban iban2("HU", string("11764379-40892883"), true);
    std::cout << "IBAN: " << iban2 << std::endl;

    Iban iban3("HU", string("117"), string("6437"), string("94089288300000000"), true);
    std::cout << "IBAN: " << iban3 << std::endl;

    try
    {
        Iban iban4("HU", string("11764379-40892880"), true);
        std::cout << "IBAN: " << iban4 << std::endl;
    }
    catch (Iban_error const& e)
    {
        std::cout << e.what() << std::endl;
    }

    Iban iban_sk_1("SK09 1100 0000 1987 4263 7541", false, true);
    std::cout << "IBAN: " << iban_sk_1 << std::endl;

    Iban iban_sk_2("SK", string(" 19-8742637541/1100 "), true);
    std::cout << "IBAN: " << iban_sk_2 << std::endl;

    Iban iban_cz_1("CZ6508000000192000145399", false, true);
    std::cout << "IBAN: " << iban_cz_1 << std::endl;

    Iban iban_cz_2("CZ", string(" 19-2000145399/0800 "), true);
    std::cout << "IBAN: " << iban_cz_2 << std::endl;

    Iban iban_at_1("AT57 3200 0000 5067 5446", false, true);
    std::cout << "IBAN: " << iban_at_1 << std::endl;

    Iban iban_at_2("AT", string(" 32000 - 00050 675 446 "), true);
    std::cout << "IBAN: " << iban_at_2 << std::endl;

    Iban iban_at_3("AT", string(" 32000 - 50 675 446 "), true);
    std::cout << "IBAN: " << iban_at_3 << std::endl;

    Iban iban_be_1("BE32.7310.0602.6402", false, true);
    std::cout << "IBAN: " << iban_be_1 << std::endl;

    Iban iban_be_2("BE", string(" 731-0060264-02 "), true);
    std::cout << "IBAN: " << iban_be_2 << std::endl;

    Iban iban_be_3("BE", string(" 731-60264-02 "), true);
    std::cout << "IBAN: " << iban_be_3 << std::endl;

    Iban iban_bg_1("BG68UBBS80023300136138", false, true);
    std::cout << "IBAN: " << iban_bg_1 << std::endl;

    Iban iban_bg_2("BG", string("UBBS 8002 3300 1361 38"), true);
    std::cout << "IBAN: " << iban_bg_2 << std::endl;

    Iban iban_fr_1("FR46 2004 1010 0721 3471 1L03 827", false, true);
    std::cout << "IBAN: " << iban_fr_1 << std::endl;

    Iban iban_fr_2("FR", string("10278 06016 00030880941 69"), true);
    std::cout << "IBAN: " << iban_fr_2 << std::endl;

    Iban iban_it_1("IT 92 T076 0112 1000 0001 5827 355 ", false, true);
    std::cout << "IBAN: " << iban_it_1 << std::endl;

    Iban iban_it_2("IT", string("T 07601 12100 000015827355"), true);
    std::cout << "IBAN: " << iban_it_2 << std::endl;

    const vector<string> de_ibans = {
    "DE84 3755 1440 0100 0002 07",  // 00
    "DE56 5605 1790 0012 1006 08",  // 00
    "DE15 40153768 0008111403",     // 01
    "DE 40664500500006114029",      // 03
    "DE94 5205 0353 0002 1862 78",  // 05
    "DE29 3906 0180 0000 3541 63",  // 06
    "DE67 3546 1106 0000 4532 85",  // 06
    "DE 10 1009 0000 2271554004",   // 06
    "DE43 5489 1300 0122 1169 72",  // 06
    "DE35 4005 0000 0000 0601 29",  // 08
    "DE76100101239623202530",       // 09
    "DE43310108333143218414",       // 09
    "DE35 2019 0003 0000 1180 01",  // 10
    "DE71 7435 0000 0003 6038 49",  // 11
    "DE26 1004 0000 0266 7897 00",  // 13
    "DE84 5624 0050 0140 1777 00",  // 13
    "DE43 2724 0004 0574 3380 00",  // 13
    "DE68 2012 0700 3100 7555 55",  // 16
    "DE60 2012 0700 3100 2949 49",  // 16
    "DE41 7001 2300 6030 4348 00",  // 17
    "DE67 3052 4400 0000 8503 97",  // 18
    "DE56 5005 0201 1252 1708 72",  // 19 (according to ibancalculator.com), but 96 according to generated_de.json
    "DE65500502220000001240",       // 19
    "DE38500502220000001241",       // 19
    "DE19130500000605015007",       // 20
    "DE84 5905 1090 0000 6018 88",  // 21
    "DE07 2685 1620 0000 0070 78",  // 22
    "DE67 1001 0010 1810 0693 01",  // 24
    "DE66 2501 00300010009309",     // 24
    "DE82 2501 0030 0000138301",    // 24
    "DE04250100301306118605",       // 24
    "DE74250100303307118608",       // 24
    "DE36250100309307118603",       // 24
    "DE70 4205 0001 0101 0486 45",  // 25
    "DE65 7201 2300 0642 5836 00",  // 26
    "DE35720123000550102400",       // 26
    "DE11590500000009212788",       // 27
    "DE38590500004567123477",       // 27 - Variante M10H
    "DE46810104002847169488",       // 27 - Variante M10H
    "DE18 1606 2008 6303 3364 00",  // 28
    "DE35160620081999900345",       // 28
    "DE31 2506 9168 9130 0002 01",  // 28
    "DE07290500002002134525",       // 29
    "DE42290500003145863029",       // 29
    "DE07 5242 0600 8107 2501 16",  // 30
    "DE67 5242 0600 0050 0030 12",  // 30
    "DE91 5032 4000 3000 0027 82",  // 31
    "DE12 5032 4000 0263 1601 65",  // 31
    "DE37 5056 0102 0000 0772 83",  // 32
    "DE40 5066 1639 0009 1414 05",  // 32
    "DE47 5136 1021 1709 1079 83",  // 32
    "DE82 2006 9125 0008 4091 11",  // 33
    "DE23400602650001111300",       // 34
    "DE87 5606 1151 0000 1000 10",  // 38
    "DE10 5606 1472 0000 0360 03",  // 38
    "DE71 5606 1472 0006 7002 60",  // 38
    "DE 60350603868621350000",      // 40
    "DE43 2502 0600 3143 2184 14",  // 41 - by BLZ + account, the ibancalculator.com shows: DE43310108333143218414 (maybe this BLZ is not in use anymore)
    "DE35 6666-1454 0000 0618 32",  // 42
    "DE 18 6669 0000 0000 9660 67", // 43
    "DE76 3006 0010 2175 3928 17",  // 44
    "DE81 5031 0400 0437 7600 00",  // 46
    "DE62 2729 0087 0003 0394 10",  // 47
    "DE40260612910002002391",       // 48
    "DE74 403 619 06 4340300200",   // 49
    "DE 69 403 510 600000 11 9768", // 49
    "DE88 4035 1060 0073 5937 25",  // 49
    "DE81403510600067001537",       // 49
    "DE87 4035 1060 0072 3331 31",  // 49
    "DE61 4035 1060 0073 2905 04",  // 49
    "DE 23 4035 1060 0000 063560",  // 49
    "DE68 4035 1060 0000 0030 95",  // 49
    "DE40 4035 1060 0063 0524 35",  // 49
    "DE52 4035 1060 0007 0246 31",  // 49
    "DE66 4006 1238 8623 3330 00",  // 49
    "DE06403510600073747685",       // 49
    "DE33 2009 0745 9101 0040 01",  // 50
    "DE64 2505 0180 0000 4567 89",  // A3
    "DE72 5625 0030 0000 0192 75",  // B2
    };

    for (auto const& iban_account : de_ibans)
    {
        try
        {
            Iban iban_de(iban_account, false, true);
            std::cout << "IBAN: " << iban_de << std::endl;
        }
        catch (Iban_error const& e)
        {
            std::cout << "IBAN: " << iban_account << " - failed with: " << e.what() << std::endl;
        }
    }

    return 0;
}
