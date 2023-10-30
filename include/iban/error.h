/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */ 

#pragma once

#include <stdexcept>

namespace iban
{

class Iban_error : public std::logic_error
{
    public:
    Iban_error(std::string error)
    : std::logic_error(error) {};
};

} // namespace iban