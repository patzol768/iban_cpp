/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */ 

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

#include "unit_base.h"

using namespace iban;
using namespace iban::countries;
using namespace std;

void unit_iban_structure_loader(std::map<std::string, Iban_structure_entry>& entries)
{
    Iban_structure_entry entry;
    entry.bban_spec = "3!n4!n1!n15!n1!n";
    entry.iban_spec = "HU2!n3!n4!n1!n15!n1!n";
    entry.bban_length = 24;
    entry.iban_length = 28;
    entry.account_code = {7, 24};
    entry.bank_code = {0, 3};
    entry.branch_code = {3, 7};
    entry.overridden = false;

    entries.emplace(make_pair(string("HU"), move(entry)));
}

void unit_bic_loader(std::vector<Bic_repository_entry>& entries)
{
    Bic_repository_entry entry;
    entry.country_code = "HU";
    entry.bank_code = "11764379";
    entry.short_name = "OTP Gy\u0151r-Moson-S. m. K\u00fclf\u00f6ldiek Deviz\u00e1i";
    entry.name = "OTP Gy\u0151r-Moson-S. m. K\u00fclf\u00f6ldiek Deviz\u00e1i";
    entry.bic = "OTPVHUHB";
    entry.primary = true;

    entries.emplace_back(move(entry));
}

TEST_CASE("IBAN from diverse formats", "[iban]")
{
    Iban iban1("HU05 1176 4379 4089 2883 0000 0000", false, true);

    REQUIRE(iban1.is_valid() == true);
    REQUIRE(iban1.get_country_code() == "HU");
    REQUIRE(iban1.get_bban() == "117643794089288300000000");
    REQUIRE(iban1.get_bban_f() == "11764379-40892883-00000000");
    REQUIRE(iban1.get_bban_t() == "1176437940892883");
    REQUIRE(iban1.get_bban_tf() == "11764379-40892883");

    Iban iban2("HU", string("11764379-40892883"), true);

    REQUIRE(iban2.is_valid() == true);
    REQUIRE(iban2.get_bban() == "117643794089288300000000");
    REQUIRE(iban2.get_country_code() == "HU");

    Iban iban3("HU", string("117"), string("6437"), string("94089288300000000"), true);

    REQUIRE(iban3.is_valid() == true);
    REQUIRE(iban3.get_bban() == "117643794089288300000000");
    REQUIRE(iban3.get_country_code() == "HU");
}

TEST_CASE("Invalid IBAN", "[iban]")
{
    Iban iban1("XX05 1176 4379 4089 2883 0000 0000 9999", true, false);

    REQUIRE(iban1.is_valid() == false);
    REQUIRE(iban1.is_valid_country_code() == false);
    REQUIRE(iban1.is_valid_structure() == true);

    Iban iban2("HU05 1176 4379 4089 2883 0000 0000 9999", true, false);

    REQUIRE(iban2.is_valid() == false);
    REQUIRE(iban2.is_valid_country_code() == true);
    REQUIRE(iban2.is_valid_length() == false);
    REQUIRE(iban2.is_valid_bban() == false);
}

// ===========================================================================

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

int main(int argc, char* argv[])
{
    // Catch::Session session; // There must be exactly one instance

    init_logger();

    Bic_repository::get_instance()->load(&unit_bic_loader);
    Iban_structure_repository::get_instance()->load(&unit_iban_structure_loader);
    REGISTER_ALL_HANDERS();

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}
