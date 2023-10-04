
#include "iban/bic.h"
#include "iban/country.h"
#include "iban/error.h"

#include <iostream>
#include <string>

using namespace iban;
using namespace std;

int main(const int parc, const char** pars)
{
    (void)parc;
    (void)pars;

    auto result = Country_repository::get_instance()->is_country("HU");
    std::cout << result << std::endl;

    return 0;
}
