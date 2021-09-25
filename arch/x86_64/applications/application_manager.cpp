#include <stl/cstdlib/cstdint.h>
#include <stl/cstdlib/stdio.h>

#include <x86_64/applications/application_manager.hpp>
#include <x86_64/drivers/serial_legacy.hpp>

#include <x86_64/applications/test/main.hpp>
#include <x86_64/applications/help/main.hpp>
#include <x86_64/applications/settings/main.hpp>

#include <x86_64/settings.hpp>

#include <x86_64/checksum.hpp>

namespace firefly::applications {
    int application_checksums[256];
    int *application_addresses[256];
    uint16_t *application_access_control[256] = {0x0000, 0x0000, 0x0000};

    void registerApplications(){
        application_addresses[0] = (int *)applications::test::test_main;
        application_checksums[0] = applications::test::getc();
        firefly::kernel::io::legacy::writeTextSerial("Registered Test Command on address 0x%X with checksum %d\n\n", &applications::test::test_main, applications::test::getc());
        application_addresses[1] = (int *)applications::help::help_main;
        application_checksums[1] = applications::help::getc();
        firefly::kernel::io::legacy::writeTextSerial("Registered Help Command on address 0x%X with checksum %d\n\n", &applications::help::help_main, applications::help::getc());
        application_addresses[2] = (int *)applications::settings::settings_main;
        application_checksums[2] = applications::settings::getc();
        firefly::kernel::io::legacy::writeTextSerial("Registered Settings Command on address 0x%X with checksum %d\n\n", &applications::settings::settings_main, applications::settings::getc());

        return;
    }

    int run(const char *application, uint16_t *access_rights, char **argv){
        printf("\n");
        int checksum = firefly::kernel::checksum::checksum(application);
        
        uint8_t temp_pointer = 0;
        while(temp_pointer < 255){
            if((application_checksums[temp_pointer] == checksum && ((uint16_t *)application_access_control[temp_pointer] <= (uint16_t *)access_rights || application_access_control[temp_pointer] == access_rights)) && (firefly::kernel::settings::get::disable_app_access_rights() == 0xff)) {
                int result = ((int (*)(int, char **))application_addresses[temp_pointer])(sizeof(argv), argv);

                return result;
            }

            if (application_checksums[temp_pointer] == checksum){
                int result = ((int (*)(int, char **))application_addresses[temp_pointer])(sizeof(argv), argv);

                return result;
            }
            temp_pointer++;
        }
        return 0x44f9ad;
    }
}