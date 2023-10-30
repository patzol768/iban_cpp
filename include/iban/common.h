/*
 * Copyright (c) 2023 Zoltan Patocs
 *
 * Licensed under the MIT License (the "License"). You may not use
 * this file except in compliance with the License. You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://opensource.org/license/mit/
 */ 

#pragma once

#include <iostream>

namespace iban
{

// enum class Bool_tri_state
// {
//     FALSE,
//     UNDECIDED,
//     TRUE
// };

}

#define SHOW_SMATCH(x)                                             \
    std::cout << "n= " << x.size() << std::endl;                   \
    for (auto i = 0; i < x.size(); ++i)                            \
    {                                                              \
        std::cout << "[" << i << "]: " << x[i].str() << std::endl; \
    }
