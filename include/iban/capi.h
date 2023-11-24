#ifndef __iban_capi_h__
#define __iban_capi_h__

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void* IBAN_PTR;

    // Initializes the library with default loaders and all countries
    // Expects the IBAN_BANK_REGISTRY and IBAN_REGISTRY and LOG4CPLUS_CONFIG
    // environment variables.
    bool iban_init();

    IBAN_PTR iban_new(char* iban, bool allow_invalid, bool validate_bban);
    IBAN_PTR iban_from_bban(char* country, char* bban, bool validate_bban);

    void iban_free(IBAN_PTR iban_ptr);

    // Validate the structural integrity of this BIC.
    bool iban_is_valid(IBAN_PTR iban_ptr);

    // Validate length.
    bool iban_is_valid_length(IBAN_PTR iban_ptr);

    // Validate structure. If it contains only allowed chars.
    bool iban_is_valid_structure(IBAN_PTR iban_ptr);

    // Validate country code.
    bool iban_is_valid_country_code(IBAN_PTR iban_ptr);

    // Validates the iban checksum
    bool iban_is_valid_iban_checksum(IBAN_PTR iban_ptr);

    // Validates the bban
    bool iban_is_valid_bban(IBAN_PTR iban_ptr);

    // Returns the whole IBAN, without separators
    char* iban_get_iban(IBAN_PTR iban_ptr);

    // Returns the whole IBAN, formatted
    char* iban_get_iban_f(IBAN_PTR iban_ptr);

    // Returns the country code from the IBAN
    char* iban_get_country_code(IBAN_PTR iban_ptr);

    // Returns the IBAN checksum
    char* iban_get_iban_checksum(IBAN_PTR iban_ptr);

    // Returns the national bank code
    char* iban_get_bankcode(IBAN_PTR iban_ptr);

    // Returns the national branch code (emtpy if no branch info)
    char* iban_get_branchcode(IBAN_PTR iban_ptr);

    // Returns the account identifier inside the bank+branch
    char* iban_get_account(IBAN_PTR iban_ptr);

    // Returns the BBAN from the IBAN
    char* iban_get_bban(IBAN_PTR iban_ptr);

    // Returns the BBAN from the IBAN, in short format if possible
    char* iban_get_bban_t(IBAN_PTR iban_ptr);

    // Returns the BBAN from the IBAN, formatted for presentation
    char* iban_get_bban_f(IBAN_PTR iban_ptr);

    // Returns the BBAN from the IBAN, formatted for presentation, in short format if possible
    char* iban_get_bban_tf(IBAN_PTR iban_ptr);

#ifdef __cplusplus
}
#endif

#endif