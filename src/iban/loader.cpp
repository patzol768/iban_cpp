/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/loader.h"
#include "iban/error.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <stdexcept>

namespace fs = std::filesystem;

using fs::directory_iterator;
using fs::path;
using iban::Iban_error;
using nlohmann::json;
using std::exception;
using std::make_move_iterator;
using std::map;
using std::string;
using std::vector;

namespace iban
{

static void readenv(string const& env, string& var)
{
    auto str = getenv(env.c_str());
    if (str != NULL && str[0] != '\0')
    {
        var.assign(str);
    }
}

void bic_loader(std::vector<Bic_repository_entry>& v)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("json"));

    try
    {
        string in_directory = "registry/bank_registry/";
        readenv("IBAN_BANK_REGISTRY", in_directory);

        for (const auto& entry : directory_iterator(in_directory))
        {
            path file_info = entry.path();
            auto file_ext = file_info.extension();

            if (strcasecmp(file_ext.c_str(), ".json"))
            {
                LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("File skipped: " << entry.path()));

                // extension not matching, skip this file
                continue;
            }


            LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("Processing: " << entry.path()));

            std::ifstream in(entry.path());
            try
            {
                auto j = json::parse(in);

                // converting JSON to objects
                vector<Bic_repository_entry> in_values = j;

                v.insert(v.end(), make_move_iterator(in_values.begin()), make_move_iterator(in_values.end()));
            }
            catch (json::exception const& e)
            {
                LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Error in: " << entry.path() << " - " << e.what()));
                // TODO: print an error
            }
        }
    }
    catch (exception const& e)
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Error in bic_loader(): " << e.what()));
        throw;
    }
}

void iban_structure_loader(std::map<std::string, Iban_structure_entry>& v)
{
    log4cplus::Logger logger = log4cplus::Logger::getInstance(LOG4CPLUS_TEXT("json"));

    try
    {

        string in_directory = "registry/iban_registry/";
        readenv("IBAN_REGISTRY", in_directory);

        string in_generated = in_directory + "generated.json";
        string in_override = in_directory + "overwrite.json";

        LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT("Processing: \"" << in_generated << "\""));

        std::ifstream in_g(in_generated);
        if (!in_g.good())
        {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("File not readable: " << in_generated));
            throw Iban_error("file missing");
        }

        try
        {
            auto j = json::parse(in_g);

            // converting JSON to objects
            map<string, Iban_structure_entry> in_values = j;

            // extract removes the element, hence the next element to process would be the first one again
            for (auto it = in_values.begin(); it != in_values.end(); it = in_values.begin())
            {
                auto e = in_values.extract(it);

                v.insert(std::move(e));
            }
        }
        catch (json::exception const& e)
        {
            LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Error in: " << in_generated << " - " << e.what()));
            throw;
        }

        std::ifstream in_o(in_override);
        if (in_o.good())
        {
            try
            {
                auto j = json::parse(in_o);

                // converting JSON to objects
                map<string, Iban_structure_entry> in_values = j;

                for (auto& e : in_values)
                {
                    auto it = v.find(e.first);
                    if (it != v.end())
                    {
                        it->second.override(e.second);
                    }
                    else
                    {
                        // definition for additional country
                        v.insert(std::move(e));
                    }
                }
            }
            catch (json::exception const& e)
            {
                LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Error in: " << in_override << " - " << e.what()));
                throw;
            }
        }
        else
        {
            LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT("File not readable: " << in_override));
        }

        // TODO: make check on input data
        //       * iban length has to be bban length + 4
        //       * bank, branch and account codes
        //         - shall not overlap
        //         - shall not leave any space in between
        //         - must precisely fit the bban length
    }
    catch (exception const& e)
    {
        LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT("Error in iban_structure_loader(): " << e.what()));
        throw;
    }
}

//   {
//     "country_code": "AE",
//     "primary": true,
//     "bic": "CBAUAEAAXXX",
//     "bank_code": "001",
//     "name": "Central Bank of UAE",
//     "short_name": "Central Bank of UAE"
//   }
void from_json(json const& j, Bic_repository_entry& e)
{
    e.bic = j.at("bic").get<string>();
    e.name = j.at("name").get<string>();
    e.primary = j.at("primary").get<bool>();
    e.bank_code = j.at("bank_code").get<string>();
    e.short_name = j.at("short_name").get<string>();
    e.country_code = j.at("country_code").get<string>();

    if (j.find("checksum_algo") != j.end())
    {
        e.checksum_algo = j.at("checksum_algo").get<string>();
    }

    if (j.find("address") != j.end())
    {
        e.address = j.at("address").get<string>();
    }
}

//   {
//     "AD": {
//       "bban_spec": "4!n4!n12!c",
//       "iban_spec": "AD2!n4!n4!n12!c",
//       "bban_length": 20,
//       "iban_length": 24,
//       "positions": {
//         "account_code": [8, 20],
//         "bank_code": [0, 4],
//         "branch_code": [4, 8]
//       }
//     }
//   }
void from_json(json const& j, Iban_structure_entry& e)
{
    e.bban_spec = (j.find("bban_spec") != j.end()) ? j.at("bban_spec").get<string>() : "";

    e.iban_spec = (j.find("iban_spec") != j.end()) ? j.at("iban_spec").get<string>() : "";

    e.bban_length = (j.find("bban_length") != j.end()) ? j.at("bban_length").get<size_t>() : 0;

    e.iban_length = (j.find("iban_length") != j.end()) ? j.at("iban_length").get<size_t>() : 0;

    if (j.find("positions") != j.end())
    {
        auto jp = j.at("positions");

        e.account_code = {0, 0};
        if (jp.find("account_code") != jp.end())
        {
            auto v = jp.at("account_code").get<vector<int>>();
            if (v.size() == 2)
            {
                e.account_code = {v[0], v[1]};
            }
        }

        e.bank_code = {0, 0};
        if (jp.find("bank_code") != jp.end())
        {
            auto v = jp.at("bank_code").get<vector<int>>();
            if (v.size() == 2)
            {
                e.bank_code = {v[0], v[1]};
            }
        }

        e.branch_code = {0, 0};
        if (jp.find("branch_code") != jp.end())
        {
            auto v = jp.at("branch_code").get<vector<int>>();
            if (v.size() == 2)
            {
                e.branch_code = {v[0], v[1]};
            }
        }
    }
    e.overridden = false;
}

} // namespace iban
