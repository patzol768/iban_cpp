# Check your IBANs easily

This is one another library for checking [IBANs](https://en.wikipedia.org/wiki/International_Bank_Account_Number). Inspired by (and some parts taken from) [schwifty](https://github.com/mdomke/schwifty), this one also provides country specific checks above the standard mod-97-10 algorithm mandated by the IBAN standard. This additional validation layer furhter decreases the chance of fooled by a wrong IBAN.

It has some support for conversion between the naional account number formats and the IBAN format, but this is by no means complete. Actually there are some countries, where without additional data (like sort code conversion list or something else) the conversion cannot be done.

The [ECBS TR 201](https://www.ecbs.org/Download/Tr201v3.9.pdf) is an invaluable source of information, though the newest accessible verion seem to be on a Czech site, [here](https://www.cnb.cz/export/sites/cnb/cs/platebni-styk/.galleries/iban/download/TR201.pdf).

Also there are a lot of IBAN libraries, with varying level of functionality. For example the [php-iban](https://github.com/globalcitizen/php-iban.git) helped a lot to start writing the German validator. A number of other sources was also checked, the related credits were put into the source files.

# State

| Country | Bank code validation | Check BBAN | Convert from national | Print national | State |
| ------- | -------------------- | ---------- | --------------------- | -------------- | ------|
| AD - Andorra                   | no  | no  | no  | no  | - |
| AT - Austria                   | yes | no  | yes | yes | - |
| BA - Bosnia and Herzegovina    | yes | no  | yes | yes | Bank specific check algorithms. List needed. |
| BE - Belgium                   | yes | yes | yes | yes | - |
| BG - Bulgaria                  | yes | no  | yes | yes | Bank specific check algorithms. List needed. Manual bank list. |
| CH - Switzerland               | yes | no  | yes | yes | Bank specific check algorithms. List needed. Manual bank list. |
| CY - Cyprus                    | yes | no  | no  | no  | Bank specific check algorithms. List needed. Manual bank list. |
| CZ - Czech republic            | yes | yes | yes | yes | - |
| DE - Germany                   | yes | yes | yes | yes | All algorithms implemented. |
| DK - Denmark                   | yes | no  | no  | no  | Separate files for structural validation. Not generally available outside Denmark. |
| EE - Estonia                   | yes | yes | yes | yes | - |
| ES - Spain                     | yes | yes | yes | yes | Generated bank list is not full. |
| FI - Finland                   | yes | yes | yes | yes | - |
| FR - France                    | yes | yes | yes | yes | Manual bank list. |
| GI - Gibraltar                 | no  | no  | no  | no  | Bank specific formats. List needed. |
| GR - Greece                    | no  | no  | no  | no  | Bank specific formats. List needed. |
| HR - Croatia                   | yes | yes | yes | yes | - |
| HU - Hungary                   | yes | yes | yes | yes | - |
| IE - Ireland                   | no  | no  | yes | yes | Bank specific check algorithms. List needed. |
| IS - Iceland                   | yes | no  | yes | yes | Manual bank list. |
| IT - Italy                     | yes | yes | yes | yes | - |
| LI - Liechtenstein             | no  | no  | no  | yes | Bank specific check algorithms. List needed. |
| LT - Lithuania                 | yes | n/a | n/a | n/a | IBAN format is the national format. |
| LU - Luxembourg                | yes | n/a | n/a | n/a | IBAN format is the national format. |
| LV - Latvia                    | yes | n/a | n/a | n/a | IBAN format is the national format. |
| ME - Montenegro                | no  | yes | yes | yes | - |
| MK - Macedonia                 | no  | yes | yes | no  | - |
| MT - Malta                     | no  | no  | yes | yes | Bank specific check algorithms. List needed. No Bank list. |
| NL - The Netherlands           | yes | yes | yes | yes | Except INGB. |
| NO - Norway                    | yes | yes | yes | yes | - |
| PL - Poland                    | yes | n/a | n/a | n/a | IBAN format (without country code) is the national format. |
| PT - Portugal                  | no  | yes | yes | yes | - |
| RO - Romania                   | yes | no  | yes | yes | No information on check digits. |
| RS - Serbia                    | yes | yes | yes | yes | RS35 only (CS73 is obsolete). |
| SE - Sweden                    | yes | yes | yes | part | Method2 IBANs loose the clearing code. |
| SI - Slovenia                  | yes | yes | yes | yes | - |
| SK - Slovak Republic           | yes | yes | yes | yes | - |
| TR - Turkey                    | yes | no  | yes | yes | Manual bank list. |
| UK - United Kingdom            | no  | no  | yes | yes | Check algorithm descriptions available, but not yet implemented / ported. |

# Links

Here are some useful links:
* BBAN checksum summary: [https://github.com/globalcitizen/php-iban/issues/39](https://github.com/globalcitizen/php-iban/issues/39)
* SK IBAN calculator: [https://nbs.sk/en/payments/general-information/iban/iban-calculator/](https://nbs.sk/en/payments/general-information/iban/iban-calculator/)
* CZ IBAN calculator: [https://www.cnb.cz/en/payments/iban/iban-calculator-czech-republic/](https://www.cnb.cz/en/payments/iban/iban-calculator-czech-republic/)
* FR IBAN calculator: [https://www.dcode.fr/bban-check](https://www.dcode.fr/bban-check)
* IBAN formats, with country specific BBAN name parts: [https://www.mutuissimo.it/iban.asp#](https://www.mutuissimo.it/iban.asp#)
* Country specific algorithms:
  * [https://pear.php.net/manual/en/package.validate.validate-fi.validate-fi.bankaccount.php](https://pear.php.net/manual/en/package.validate.validate-fi.validate-fi.bankaccount.php)
  * [https://pear.php.net/packages.php?catpid=50&catname=Validate](https://pear.php.net/packages.php?catpid=50&catname=Validate)

# Folder structure

* include, src: as usual
* scripts: these scripts are downloading the publised data (bank list, iban formats) and generating the registries.
* regisry
  * bank_registry: country specific files. Can be either created automatically (generated_*), or by hand (manual_*)
  * iban_registry: contains the IBAN formats for all related countries (generated.json) also an auxiliary file to fix the errors in the format specification (overwrite.json).

# How to use?

The library itself is written in C++, but also C binding is available. Check the tests for more elaborate samples.

## C++

### Initialize the library

```c++
//
// Bank codes and IBAN syntax are available in separate files
// Those are almost the same as in schwifty. See formats there.
//
// The loaders expect a callback function to ease the local integration.
//

// Loads the bank codes
    Bic_repository::get_instance()->load(&bic_loader);

// Loads the IBAN structure definitions
    Iban_structure_repository::get_instance()->load(&iban_structure_loader);

// Each country has its own handler to deal with the local account specific details.
// One could select the required ones, or simply register all available.
//
// IBAN validation happens even without the country specific handlers. Similarl to
// the already referred schwifty.
//
    REGISTER_ALL_HANDERS();

```

### Checking an IBAN

```c++
    try
    {
        // IBANs accepted with or without separators.
        //
        // Invalid ibans normally result in exception, but you can force the library
        // to accept invalid ones. (Useful to try to extract possible bank / branch
        // info. Although that could be higly unreliable.)
        //
        // Note that this library does not support heuristics to try fixing potential
        // mistyping.
        //
        // The last parameter forces the BBAN check if possible. (With a country
        // specific handler.) - Note that some handlers skip the in depth validation
        // and simply mark the account valid. Be aware of this risk.
        Iban iban("HU05 1176 4379 4089 2883 0000 0000", false, true);
        std::cout << "IBAN: " << iban << std::endl;
    }
    catch (Iban_error const& e)
    {
        std::cout << "IBAN: " << iban_account << " - failed with: " << e.what() << std::endl;
    }
```

### Converting to IBAN

```c++
    std::string country = "BE";
    std::string local_account = " 731-60264-02 ";
    
    try
    {
        // The country code and the local account together is normally enough
        // to convert into an IBAN. This conversion needs the country specific
        // handler.
        //
        // When there's no country specific handler, the library would still
        // create an "IBAN looking string", although that string won't be a
        // valid IBAN. Or maybe accidentally.
        //
        Iban iban(country, local_account, true);
        std::cout << "IBAN: " << iban << std::endl;
    }
    catch (Iban_error const& e)
    {
        std::cout << "BBAN: " << country << " " << local_account << " - failed with: " << e.what() << std::endl;

        // The BBAN validation skipped in this case. Yet one could check how it
        // would look if it were not invalid on BBAN level. 
        Iban iban_f(country, local_account, false);
        std::cout << "IBAN: " << iban_f << std::endl;
    }
```

### Converting to IBAN from account parts

IBAN structure definition normally contains the bank, branch and account parts; suggesting that one could easily create an IBAN when these are known. Although this is not true in reality, the library provides such a functionality for the brave ones.

```c++
    Iban iban_mt("MT", string("MMEB"), string("44392"), string("39011176002"), true);
    std::cout << "IBAN: " << iban_mt << std::endl;
```

## C

There's a very small wrapper around the methods of the IBAN class. This way one can use in C projects (and many others).

Note that the log4cplus library would be needed for linking, since the default data readers are using that. For compile only one single header is needed. (There's a test written in C, as a reference.)

```c
    #include "iban/capi.h"

    accno = "RO34 UGBI 0000 3620 0627 7RON";

    iban = iban_new(accno, false, true);
    if (!iban)
        goto err;

    str = iban_get_iban(iban); // free() when not needed anymore
    printf("%s\n", str);
```

# Extension

Ultimately the best way to extend this library is to contribute with fixes, country specific validations, etc. Here are some highlights on the possible extensions.

## New handlers

Country specific handlers are contain a sizeable part of the validation logic. For most of the cases those should just fit. Yet in some cases one may want to add some more validation. Here is the way to do:

```c++
// Let us add some local validation to the Hungarian country specific handler.

namespace iban
{
namespace countries
{

// is_valid_ext() is a weak function, hence one can add an own validation algorithm
bool BBan_handler_HU::is_valid_ext(std::string const& bban) const
{
    std::cout << "my validator" << std::endl;
    return true;
}

}
}
```

## New bank lists

Country specific bank lists are essential to make some deeper validation of IBAN, than the checksum. By default the library contains the python based generators used by [schwifty](https://github.com/mdomke/schwifty), but it is never too late to extend the list.

In case the given country is not publicly publishing the list of Banks in an electronic format, than it is still better to have a manually collected list, then nothig. A sample record look like:

```json
  {
    "country_code": "BG",
    "bank_code": "UBBS",
    "short_name": "UNITED BULGARIAN BANK AD",
    "name": "UNITED BULGARIAN BANK AD",
    "bic": "UBBSBGSF",
    "primary": true
  }
```

## Fixes to the IBAN registry

[Schwifty](https://github.com/mdomke/schwifty) has a generator to collect the country specific IBAN format information. As it looks, either the information source or the generator has some issues, resulting in invalid values in the registry. Similary as [schwifty](https://github.com/mdomke/schwifty), this library also provides a way to manually fix these values. For example:

```json
  "PL": {
    "positions": {
      "account_code": [8, 24],
      "bank_code": [0, 8],
      "branch_code": [0, 0]
    }
  }
```

# Some notes

There are certainly bugs. Feel free to fix or just notify me, although I cannot promise anything.

I was trying to use adequately licensed materials and parts. Yet if you feel any possible license issues, please contact.

Feel free to use anywhere. If you find this useful, than please consider some contribution in order to make it even more appealing and useful for others.
