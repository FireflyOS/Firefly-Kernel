#pragma once

#include <stl/cstdlib/cstdint.h>

namespace firefly::applications {
    void registerApplications();
    int run(const char application[]);
}