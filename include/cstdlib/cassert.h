#pragma once

#include "firefly/panic.hpp"

#define assert(condition) assert_truth(condition)

#define assert_truth(condition)                           \
    do {                                                  \
        if (!(condition))                                 \
            firefly::assertion_failure_panic(#condition); \
    } while (0)
