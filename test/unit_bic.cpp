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
using namespace std;

void unit_bic_loader(std::vector<Bic_repository_entry>& entries)
{
    Bic_repository_entry entry_1;
    entry_1.country_code = "HU";
    entry_1.bank_code = "11764379";
    entry_1.short_name = "OTP Gy\u0151r-Moson-S. m. K\u00fclf\u00f6ldiek Deviz\u00e1i";
    entry_1.name = "OTP Gy\u0151r-Moson-S. m. K\u00fclf\u00f6ldiek Deviz\u00e1i";
    entry_1.bic = "OTPVHUHB";
    entry_1.primary = true;

    entries.emplace_back(move(entry_1));

    Bic_repository_entry entry_2;
    entry_2.country_code= "HU";
    entry_2.bank_code= "10200012";
    entry_2.short_name= "K&H Bank Zrt. 242 P\u00e9cs";
    entry_2.name= "K&H Bank Zrt. 242 P\u00e9cs";
    entry_2.bic= "OKHBHUHB";
    entry_2.primary= true;

    entries.emplace_back(move(entry_2));

    Bic_repository_entry entry_3;
    entry_3.country_code="HU";
    entry_3.bank_code="10400023";
    entry_3.short_name="K&H Bank Zrt. 002 Budapest";
    entry_3.name="K&H Bank Zrt. 002 Budapest";
    entry_3.bic="OKHBHUHB";
    entry_3.primary=true;

    entries.emplace_back(move(entry_3));

    Bic_repository_entry entry_4;
    entry_4.country_code="HU";
    entry_4.bank_code="88800015";
    entry_4.short_name="K&H Jelz\u00e1logbank Zrt.";
    entry_4.name="K&H Jelz\u00e1logbank Zrt.";
    entry_4.bic="OKHBHUHB";
    entry_4.primary=true;

    entries.emplace_back(move(entry_4));

    Bic_repository_entry entry_5;
    entry_5.bank_code="10000000";
    entry_5.name="Bundesbank";
    entry_5.short_name="BBk Berlin";
    entry_5.bic="MARKDEF1100";
    entry_5.primary=true;
    entry_5.country_code="DE";
    entry_5.checksum_algo="09";

    entries.emplace_back(move(entry_5));

    Bic_repository_entry entry_6;
    entry_6.bank_code="87070024";
    entry_6.name="Deutsche Bank";
    entry_6.short_name="Deutsche Bank";
    entry_6.bic="DEUTDEDBCHE";
    entry_6.primary=true;
    entry_6.country_code="DE";
    entry_6.checksum_algo="63";

    entries.emplace_back(move(entry_6));

    Bic_repository_entry entry_7;
    entry_7.bank_code="87070024";
    entry_7.name="Deutsche Bank";
    entry_7.short_name="Deutsche Bank";
    entry_7.bic="DEUTDEDB871";
    entry_7.primary=false;
    entry_7.country_code="DE";
    entry_7.checksum_algo="63";

    entries.emplace_back(move(entry_7));
}

TEST_CASE("Valid BIC from repository", "[bic]")
{
    auto result = Bic_repository::get_instance()->get_by_bic("OKHBHUHB");
    REQUIRE(result.size() == 3);
}

TEST_CASE("Valid long BIC from repository", "[bic]")
{
    auto result = Bic_repository::get_instance()->get_by_bic("OKHBHUHBXXX");
    REQUIRE(result.size() == 3);
}

TEST_CASE("Invalid BIC from repository", "[bic]")
{
    auto result = Bic_repository::get_instance()->get_by_bic("XXXXHUXX");
    REQUIRE(result.size() == 0);
}

TEST_CASE("Valid BIC, check lots of data", "[bic]")
{
    Bic bic("OKHBHUHBXXX");
    REQUIRE(bic.get_formatted() == "OKHB HU HB XXX");
    REQUIRE(bic.get_domestic_bank_codes().size() == 3);
    REQUIRE(bic.get_names().size() == 3);
    REQUIRE(bic.get_short_names().size() == 3);
    REQUIRE(bic.get_type() == Bic_type::DEFAULT);
    REQUIRE(bic.get_short_code() == "OKHBHUHB");
    REQUIRE(bic.get_country() == "Hungary");
    REQUIRE(bic.get_bank_code() == "OKHB");
    REQUIRE(bic.get_country_code() == "HU");
    REQUIRE(bic.get_location_code() == "HB");
    REQUIRE(bic.get_branch_code().value_or("") == "XXX");
}

TEST_CASE("Non-existing BIC, but seemingly valid", "[bic]")
{
    Bic bic("AAAAHUDDXXX");
    REQUIRE(bic.is_valid() == true);
    REQUIRE(bic.is_valid_length() == true);
    REQUIRE(bic.is_valid_structure() == true);
    REQUIRE(bic.is_valid_country_code() == true);
    REQUIRE(bic.is_valid_iban_country_code() == true);
    REQUIRE(bic.is_valid_sepa_country_code() == true);
    REQUIRE(bic.is_existing() == false);
}

TEST_CASE("Invalid BIC, run some checks", "[bic]")
{
    Bic bic("BBBBAA__", true);
    REQUIRE(bic.is_valid() == false);
    REQUIRE(bic.is_valid_length() == true);
    REQUIRE(bic.is_valid_structure() == false);
    REQUIRE(bic.is_valid_country_code() == false);
    REQUIRE(bic.is_valid_iban_country_code() == false);
    REQUIRE(bic.is_valid_sepa_country_code() == false);
    REQUIRE(bic.is_existing() == false);
}

TEST_CASE("Valid BIC, trimming and upper case needed", "[bic]")
{
    Bic bic(" okhb hu hb xxx ");
    REQUIRE(bic.get_short_code() == "OKHBHUHB");
}

TEST_CASE("Valid country and bankcode, unique", "[bic]")
{
    Bic bic("DE", "10000000", false);
    REQUIRE(bic.get_short_code() == "MARKDEF1100");
}

TEST_CASE("Valid country and bankcode, multiple banks not allowed", "[bic]")
{
    REQUIRE_THROWS(new Bic("DE", "87070024", false));
}

TEST_CASE("Valid country and bankcode, one of multiple banks", "[bic]")
{
    Bic bic("DE", "87070024", true);
    REQUIRE(bic.get_short_code().substr(0, 8) == "DEUTDEDB");
}

TEST_CASE("Invalid country and bankcode", "[bic]")
{
    REQUIRE_THROWS(new Bic ("DE", "987654321", false));
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

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}
