
#include "iban/bban.h"
#include "iban/bic.h"
#include "iban/countries/countries.h"
#include "iban/country.h"
#include "iban/error.h"
#include "iban/loader.h"

#include <iostream>
#include <log4cplus/configurator.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <string>

using namespace iban;
using namespace iban::countries;
using namespace std;

/** Initializes logging
 *
 *  LOG4CPLUS_CONFIG environment variable shall contain the path of the log4cplus log configuration.
 *  If LOG4CPLUS_CONFIG not defined, than defaults to "log.ini"
 */
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

/*
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

} // namespace countries
} // namespace iban
*/

/** main()
 *
 */
int main(const int parc, const char** pars)
{
    (void)parc;
    (void)pars;

    init_logger();

    // auto result = Country_repository::get_instance()->is_country("HU");
    // std::cout << result << std::endl;

    Bic_repository::get_instance()->load(&bic_loader);

    // Bic bic("DE", "10000000", false);

    Iban_structure_repository::get_instance()->load(&iban_structure_loader);

    BBan_handler_factory::get_instance()->register_validator("HU", std::dynamic_pointer_cast<BBan_handler>(std::make_shared<BBan_handler_HU>()));

    Iban iban1("HU05 1176 4379 4089 2883 0000 0000", false, true);
    std::cout << "IBAN: " << iban1 << std::endl;

    Iban iban2("HU", string("11764379-40892883"), true);
    std::cout << "IBAN: " << iban2 << std::endl;

    Iban iban3("HU", string("117"), string("6437"), string("94089288300000000"), true);
    std::cout << "IBAN: " << iban3 << std::endl;

    try
    {
        Iban iban4("HU", string("11764379-40892880"), true);
        std::cout << "IBAN: " << iban4 << std::endl;
    }
    catch (Iban_error const& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
