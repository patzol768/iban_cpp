/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

#include "iban/bban.h"
#include "iban/countries/AT.h"
#include "iban/countries/BA.h"
#include "iban/countries/BE.h"
#include "iban/countries/BG.h"
#include "iban/countries/CH.h"
#include "iban/countries/CZ_SK.h"
#include "iban/countries/DE.h"
#include "iban/countries/EE.h"
#include "iban/countries/ES.h"
#include "iban/countries/FI.h"
#include "iban/countries/FR.h"
#include "iban/countries/HR.h"
#include "iban/countries/HU.h"
#include "iban/countries/IE.h"
#include "iban/countries/IS.h"
#include "iban/countries/IT.h"
#include "iban/countries/LI.h"
#include "iban/countries/LT.h"
#include "iban/countries/LU.h"
#include "iban/countries/LV.h"
#include "iban/countries/ME.h"
#include "iban/countries/MK.h"
#include "iban/countries/MT.h"
#include "iban/countries/NL.h"
#include "iban/countries/NO.h"
#include "iban/countries/PL.h"
#include "iban/countries/PT.h"
#include "iban/countries/RO.h"
#include "iban/countries/RS.h"
#include "iban/countries/SE.h"
#include "iban/countries/SI.h"
#include "iban/countries/TR.h"
#include <memory>

#define REGISTER_HANDLER(country, handler) \
    ::iban::BBan_handler_factory::get_instance()->register_handler(country, std::dynamic_pointer_cast<::iban::BBan_handler>(std::make_shared<::iban::BBan_handler>(country)))

#define REGISTER_ALL_HANDERS()                  \
    REGISTER_HANDLER("AT", ::iban::countries::BBan_handler_AT);    \
    REGISTER_HANDLER("BA", ::iban::countries::BBan_handler_BA);    \
    REGISTER_HANDLER("BE", ::iban::countries::BBan_handler_BE);    \
    REGISTER_HANDLER("BG", ::iban::countries::BBan_handler_BG);    \
    REGISTER_HANDLER("CF", ::iban::countries::BBan_handler_FR);    \
    REGISTER_HANDLER("CG", ::iban::countries::BBan_handler_FR);    \
    REGISTER_HANDLER("CH", ::iban::countries::BBan_handler_CH);    \
    REGISTER_HANDLER("CZ", ::iban::countries::BBan_handler_CZ_SK); \
    REGISTER_HANDLER("DE", ::iban::countries::BBan_handler_DE);    \
    REGISTER_HANDLER("EE", ::iban::countries::BBan_handler_EE);    \
    REGISTER_HANDLER("ES", ::iban::countries::BBan_handler_ES);    \
    REGISTER_HANDLER("DJ", ::iban::countries::BBan_handler_FR);    \
    REGISTER_HANDLER("FI", ::iban::countries::BBan_handler_FI);    \
    REGISTER_HANDLER("FR", ::iban::countries::BBan_handler_FR);    \
    REGISTER_HANDLER("GA", ::iban::countries::BBan_handler_FR);    \
    REGISTER_HANDLER("GQ", ::iban::countries::BBan_handler_FR);    \
    REGISTER_HANDLER("HU", ::iban::countries::BBan_handler_HU);    \
    REGISTER_HANDLER("HR", ::iban::countries::BBan_handler_HR);    \
    REGISTER_HANDLER("IE", ::iban::countries::BBan_handler_IE);    \
    REGISTER_HANDLER("IS", ::iban::countries::BBan_handler_IS);    \
    REGISTER_HANDLER("IT", ::iban::countries::BBan_handler_IT);    \
    REGISTER_HANDLER("LI", ::iban::countries::BBan_handler_LI);    \
    REGISTER_HANDLER("LT", ::iban::countries::BBan_handler_LT);    \
    REGISTER_HANDLER("LU", ::iban::countries::BBan_handler_LU);    \
    REGISTER_HANDLER("LV", ::iban::countries::BBan_handler_LV);    \
    REGISTER_HANDLER("KM", ::iban::countries::BBan_handler_FR);    \
    REGISTER_HANDLER("MC", ::iban::countries::BBan_handler_FR);    \
    REGISTER_HANDLER("ME", ::iban::countries::BBan_handler_ME);    \
    REGISTER_HANDLER("MK", ::iban::countries::BBan_handler_MK);    \
    REGISTER_HANDLER("MT", ::iban::countries::BBan_handler_MT);    \
    REGISTER_HANDLER("NL", ::iban::countries::BBan_handler_NL);    \
    REGISTER_HANDLER("NO", ::iban::countries::BBan_handler_NO);    \
    REGISTER_HANDLER("PL", ::iban::countries::BBan_handler_PL);    \
    REGISTER_HANDLER("PT", ::iban::countries::BBan_handler_PT);    \
    REGISTER_HANDLER("RO", ::iban::countries::BBan_handler_RO);    \
    REGISTER_HANDLER("RS", ::iban::countries::BBan_handler_RS);    \
    REGISTER_HANDLER("SE", ::iban::countries::BBan_handler_SE);    \
    REGISTER_HANDLER("SI", ::iban::countries::BBan_handler_SI);    \
    REGISTER_HANDLER("SK", ::iban::countries::BBan_handler_CZ_SK); \
    REGISTER_HANDLER("SM", ::iban::countries::BBan_handler_IT);    \
    REGISTER_HANDLER("TD", ::iban::countries::BBan_handler_FR);    \
    REGISTER_HANDLER("TR", ::iban::countries::BBan_handler_TR);
