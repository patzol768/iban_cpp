/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */ 

#pragma once

#include "iban/bic.h"
#include "iban/iban.h"

// See: https://json.nlohmann.me/home/exceptions/#extended-diagnostic-messages
// #define JSON_DIAGNOSTICS 1
#include <nlohmann/json.hpp>

namespace iban
{

void bic_loader(std::vector<Bic_repository_entry>& v);
void iban_structure_loader(std::map<std::string, Iban_structure_entry>& v);

void from_json(nlohmann::json const& j, Bic_repository_entry& e);
void from_json(nlohmann::json const& j, Iban_structure_entry& e);

} // namespace iban
