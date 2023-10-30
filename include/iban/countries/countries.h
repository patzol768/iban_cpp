#include "iban/countries/AT.h"
#include "iban/countries/BE.h"
#include "iban/countries/BG.h"
#include "iban/countries/CZ_SK.h"
#include "iban/countries/DE.h"
#include "iban/countries/FR.h"
#include "iban/countries/HU.h"
#include "iban/countries/IT.h"

#define REGISTER_HANDLER(country, handler) \
    BBan_handler_factory::get_instance()->register_handler(country, std::dynamic_pointer_cast<BBan_handler>(std::make_shared<handler>(country)))

#define REGISTER_ALL_HANDERS()                  \
    REGISTER_HANDLER("AT", BBan_handler_AT);    \
    REGISTER_HANDLER("BE", BBan_handler_BE);    \
    REGISTER_HANDLER("BG", BBan_handler_BG);    \
    REGISTER_HANDLER("CF", BBan_handler_FR);    \
    REGISTER_HANDLER("CG", BBan_handler_FR);    \
    REGISTER_HANDLER("CZ", BBan_handler_CZ_SK); \
    REGISTER_HANDLER("DE", BBan_handler_DE);    \
    REGISTER_HANDLER("DJ", BBan_handler_FR);    \
    REGISTER_HANDLER("FR", BBan_handler_FR);    \
    REGISTER_HANDLER("GA", BBan_handler_FR);    \
    REGISTER_HANDLER("GQ", BBan_handler_FR);    \
    REGISTER_HANDLER("HU", BBan_handler_HU);    \
    REGISTER_HANDLER("IT", BBan_handler_IT);    \
    REGISTER_HANDLER("KM", BBan_handler_FR);    \
    REGISTER_HANDLER("MC", BBan_handler_FR);    \
    REGISTER_HANDLER("SK", BBan_handler_CZ_SK); \
    REGISTER_HANDLER("TD", BBan_handler_FR);
