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
