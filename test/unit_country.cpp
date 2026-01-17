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

TEST_CASE("Country results", "[country]")
{
    auto result = Country_repository::get_instance()->is_country("HU");
    REQUIRE(result == true);
}

TEST_CASE("Country not results with lowercase", "[country]")
{
    auto result = Country_repository::get_instance()->is_country("at");
    REQUIRE(result == true);
}

TEST_CASE("Country not results", "[country]")
{
    auto result = Country_repository::get_instance()->is_country("A!");
    REQUIRE(result == false);
}

TEST_CASE("Country is iban country", "[country]")
{
    auto result = Country_repository::get_instance()->is_iban_country("HU");
    REQUIRE(result == true);
}

TEST_CASE("Country is not iban country", "[country]")
{
    auto result = Country_repository::get_instance()->is_iban_country("BS");
    REQUIRE(result == false);
}

TEST_CASE("Country is sepa country", "[country]")
{
    auto result = Country_repository::get_instance()->is_sepa_country("HU");
    REQUIRE(result == true);
}

TEST_CASE("Non-existing country is not sepa country", "[country]")
{
    auto result = Country_repository::get_instance()->is_iban_country("A!");
    REQUIRE(result == false);
}

TEST_CASE("Country name", "[country]")
{
    auto result = Country_repository::get_instance()->get_by_alpha2("HU");
    REQUIRE(result.has_value() == true);
    REQUIRE(result.value().get().name == "Hungary");
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

    int result = Catch::Session().run(argc, argv);

    // your clean-up...

    return result;
}
