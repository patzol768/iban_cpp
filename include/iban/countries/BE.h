/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */ 

#pragma once

#include "iban/bban.h"

namespace iban
{
namespace countries
{

class BBan_handler_BE : public BBan_handler
{
    public:
    BBan_handler_BE(std::string const& country);

    // Validate the checksum(s) inside the BBAN
    bool is_valid_checksum(std::string const& bban) const override;

    // Validate the bban with an externally provided validator (e.g. bank specific check)
    //
    // It is implemented as a weak function, hence one can replace with own code (the
    // base simply returnes true) 
    bool is_valid_ext(std::string const& bban) const override;

    // Makes BBAN suitable for IBAN (e.g. if there are short BBAN formats, etc.)
    std::string preformat(std::string const& bban) const override;

    // Cut short the BBAN, if it has both long and short format;
    std::string trim(std::string const& bban) const override;

    // Formats BBAN according to the local presentation
    std::string format(std::string const& bban) const override;
};

} // namespace countries
} // namespace iban