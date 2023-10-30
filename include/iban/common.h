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
