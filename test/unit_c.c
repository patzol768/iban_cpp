/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/capi.h"

#include <malloc.h>
#include <stdbool.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int result = 1;
    IBAN_PTR iban = NULL;
    char* accno = NULL;
    char* str = NULL;
    
    if (!iban_init()) goto err;

    accno = "RO34 UGBI 0000 3620 0627 7RON";

    iban = iban_new(accno, false, true);
    if (!iban)
        goto err;
    printf("iban created\n");

    str = iban_get_iban(iban); // free() when not needed anymore
    if (strcmp(str, "RO34UGBI0000362006277RON"))
        goto err;

    printf("IBAN is: %s\n", str);

    result = 0;
err:
    if (!str)
        free(str);
    if (!iban)
        iban_free(iban);
    return result;
}
