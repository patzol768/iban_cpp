#!/usr/bin/env bash
#set -o errexit
set -e

main() {
  local -r root=$(dirname "${BASH_SOURCE[0]}")

  echo "---> Updating iban registry"
  python3 "${root}/get_iban_registry.py"

  for country in "at" "be" "cz" "de" "es" "fi" "hr" "hu" "it" "lt" "lv" "nl" "no" "pl" "ro" "se" "si" "sk" "ua"; do
    echo "---> Updating bank registry for: ${country}"
    python3 "${root}/get_bank_registry_${country}.py"
  done
}

main
