/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */

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

#define REGISTER_HANDLER(country, handler) \
    BBan_handler_factory::get_instance()->register_handler(country, std::dynamic_pointer_cast<BBan_handler>(std::make_shared<::iban::countries::handler>(country)))

#define REGISTER_ALL_HANDERS()                  \
    REGISTER_HANDLER("AT", iban::BBan_handler_AT);    \
    REGISTER_HANDLER("BA", iban::BBan_handler_BA);    \
    REGISTER_HANDLER("BE", iban::BBan_handler_BE);    \
    REGISTER_HANDLER("BG", iban::BBan_handler_BG);    \
    REGISTER_HANDLER("CF", iban::BBan_handler_FR);    \
    REGISTER_HANDLER("CG", iban::BBan_handler_FR);    \
    REGISTER_HANDLER("CH", iban::BBan_handler_CH);    \
    REGISTER_HANDLER("CZ", iban::BBan_handler_CZ_SK); \
    REGISTER_HANDLER("DE", iban::BBan_handler_DE);    \
    REGISTER_HANDLER("EE", iban::BBan_handler_EE);    \
    REGISTER_HANDLER("ES", iban::BBan_handler_ES);    \
    REGISTER_HANDLER("DJ", iban::BBan_handler_FR);    \
    REGISTER_HANDLER("FI", iban::BBan_handler_FI);    \
    REGISTER_HANDLER("FR", iban::BBan_handler_FR);    \
    REGISTER_HANDLER("GA", iban::BBan_handler_FR);    \
    REGISTER_HANDLER("GQ", iban::BBan_handler_FR);    \
    REGISTER_HANDLER("HU", iban::BBan_handler_HU);    \
    REGISTER_HANDLER("HR", iban::BBan_handler_HR);    \
    REGISTER_HANDLER("IE", iban::BBan_handler_IE);    \
    REGISTER_HANDLER("IS", iban::BBan_handler_IS);    \
    REGISTER_HANDLER("IT", iban::BBan_handler_IT);    \
    REGISTER_HANDLER("LI", iban::BBan_handler_LI);    \
    REGISTER_HANDLER("LT", iban::BBan_handler_LT);    \
    REGISTER_HANDLER("LU", iban::BBan_handler_LU);    \
    REGISTER_HANDLER("LV", iban::BBan_handler_LV);    \
    REGISTER_HANDLER("KM", iban::BBan_handler_FR);    \
    REGISTER_HANDLER("MC", iban::BBan_handler_FR);    \
    REGISTER_HANDLER("ME", iban::BBan_handler_ME);    \
    REGISTER_HANDLER("MK", iban::BBan_handler_MK);    \
    REGISTER_HANDLER("MT", iban::BBan_handler_MT);    \
    REGISTER_HANDLER("NL", iban::BBan_handler_NL);    \
    REGISTER_HANDLER("NO", iban::BBan_handler_NO);    \
    REGISTER_HANDLER("PL", iban::BBan_handler_PL);    \
    REGISTER_HANDLER("PT", iban::BBan_handler_PT);    \
    REGISTER_HANDLER("RO", iban::BBan_handler_RO);    \
    REGISTER_HANDLER("RS", iban::BBan_handler_RS);    \
    REGISTER_HANDLER("SE", iban::BBan_handler_SE);    \
    REGISTER_HANDLER("SI", iban::BBan_handler_SI);    \
    REGISTER_HANDLER("SK", iban::BBan_handler_CZ_SK); \
    REGISTER_HANDLER("SM", iban::BBan_handler_IT);    \
    REGISTER_HANDLER("TD", iban::BBan_handler_FR);    \
    REGISTER_HANDLER("TR", iban::BBan_handler_TR);
