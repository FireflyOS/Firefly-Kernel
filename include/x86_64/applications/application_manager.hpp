#pragma once

#include <stl/cstdlib/cstdint.h>

namespace firefly::applications {
    void registerApplications();
    int run(const char *application, uint16_t *access_rights, char **argv);
    const char *get_commands();

    //Checksums
    #define settings_c 497;
    #define help_c 233;
    #define test_c 256;
}