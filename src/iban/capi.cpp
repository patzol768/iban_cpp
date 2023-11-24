/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/capi.h"
#include "iban/countries/countries.h"
#include "iban/country.h"
#include "iban/error.h"
#include "iban/iban.h"
#include "iban/loader.h"

#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <string>

#include <malloc.h>

using iban::BBan_handler;
using iban::BBan_handler_factory;
using iban::bic_loader;
using iban::Bic_repository;
using iban::Iban;
using iban::iban_structure_loader;
using iban::Iban_structure_repository;
using std::string;

#define CALL_BOOL_GETTER(x)          \
    try                              \
    {                                \
        auto iban = (Iban*)iban_ptr; \
        return x;                    \
    }                                \
    catch (...)                      \
    {                                \
        return false;                \
    }

#define CALL_STRING_GETTER(x)                              \
    try                                                    \
    {                                                      \
        auto iban = (Iban*)iban_ptr;                       \
        auto info = iban->get_iban();                      \
        auto result = (char*)malloc(info.size() + 1);      \
        if (result)                                        \
        {                                                  \
            memcpy(result, info.c_str(), info.size() + 1); \
        }                                                  \
                                                           \
        return result;                                     \
    }                                                      \
    catch (...)                                            \
    {                                                      \
        return nullptr;                                    \
    }

namespace iban
{
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
} // namespace iban

using iban::init_logger;

extern "C"
{
    bool iban_init()
    {
        try
        {
            // default loaders use log4cplus logging
            init_logger();

            Bic_repository::get_instance()->load(&bic_loader);
            Iban_structure_repository::get_instance()->load(&iban_structure_loader);

            REGISTER_ALL_HANDERS();
        }
        catch (...)
        {
            return false;
        }

        return true;
    }

    IBAN_PTR iban_new(char* iban, bool allow_invalid, bool validate_bban)
    {
        try
        {
            auto account = string(iban);
            auto iban = new Iban(account, allow_invalid, validate_bban);
            return iban;
        }
        catch (...)
        {
            return nullptr;
        }
    }

    IBAN_PTR iban_from_bban(char* country, char* bban, bool validate_bban)
    {
        try
        {
            auto country_s = string(country);
            auto account = string(bban);
            auto iban = new Iban(country_s, account, validate_bban);
            return iban;
        }
        catch (...)
        {
            return nullptr;
        }
    }

    void iban_free(IBAN_PTR iban_ptr)
    {
        try
        {
            auto iban = (Iban*)iban_ptr;
            delete iban;
            return;
        }
        catch (...)
        {
            return;
        }
    }

    bool iban_is_valid(IBAN_PTR iban_ptr)
    {
        CALL_BOOL_GETTER(iban->is_valid());
    }

    bool iban_is_valid_length(IBAN_PTR iban_ptr)
    {
        CALL_BOOL_GETTER(iban->is_valid_length());
    }

    bool iban_is_valid_structure(IBAN_PTR iban_ptr)
    {
        CALL_BOOL_GETTER(iban->is_valid_structure());
    }

    bool iban_is_valid_country_code(IBAN_PTR iban_ptr)
    {
        CALL_BOOL_GETTER(iban->is_valid_country_code());
    }

    bool iban_is_valid_iban_checksum(IBAN_PTR iban_ptr)
    {
        CALL_BOOL_GETTER(iban->is_valid_iban_checksum());
    }

    bool iban_is_valid_bban(IBAN_PTR iban_ptr)
    {
        CALL_BOOL_GETTER(iban->is_valid_bban());
    }

    char* iban_get_iban(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_iban());
    }

    char* iban_get_iban_f(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_iban_f());
    }

    char* iban_get_country_code(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_country_code());
    }

    char* iban_get_iban_checksum(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_iban_checksum());
    }

    char* iban_get_bankcode(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_bankcode());
    }

    char* iban_get_branchcode(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_branchcode());
    }

    char* iban_get_account(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_account());
    }

    char* iban_get_bban(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_bban());
    }

    char* iban_get_bban_t(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_bban_t());
    }

    char* iban_get_bban_f(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_bban_f());
    }

    char* iban_get_bban_tf(IBAN_PTR iban_ptr)
    {
        CALL_STRING_GETTER(iban->get_bban_tf());
    }
}
