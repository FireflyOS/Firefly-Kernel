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
    struct apps {
        int     *address;
        int      checksum;
        uint16_t access;
    } apps_s[256];

    void registerApplications(){
        apps_s[0].address = (int *)applications::test::test_main;
        apps_s[0].checksum = applications::test::getc();
        firefly::kernel::io::legacy::writeTextSerial("Registered Test Command on address 0x%X with checksum %d\n\n", &applications::test::test_main, applications::test::getc());

        apps_s[1].address = (int *)applications::help::help_main;
        apps_s[1].checksum = applications::help::getc();
        firefly::kernel::io::legacy::writeTextSerial("Registered Help Command on address 0x%X with checksum %d\n\n", &applications::help::help_main, applications::help::getc());

        apps_s[2].address = (int *)applications::settings::settings_main;
        apps_s[2].checksum = applications::settings::getc();
        firefly::kernel::io::legacy::writeTextSerial("Registered Settings Command on address 0x%X with checksum %d\n\n", &applications::settings::settings_main, applications::settings::getc());

        return;
    }

    int run(const char *application, uint16_t access_rights, char **argv){
        printf("\n");

        int checksum = firefly::kernel::checksum::checksum(application);

        if(firefly::kernel::settings::get::kernel_mode() == 0x9a){
            printf("Checksum: %d\n", checksum);
        }
        
        uint8_t temp_pointer = 0;
        while(temp_pointer < 255){
            if((apps_s[temp_pointer].checksum == checksum && (apps_s[temp_pointer].access <= access_rights || apps_s[temp_pointer].access == access_rights)) && firefly::kernel::settings::get::disable_app_access_rights() != 0xff) {
                int result = ((int (*)(int, char **))apps_s[temp_pointer].address)(sizeof(argv), argv);
                if (result == -1) printf("\n[ERROR] An error has occurred in the application!\nExit code: %d\n", result);
                
                return result;

            }

            if ((apps_s[temp_pointer].checksum == checksum) && firefly::kernel::settings::get::disable_app_access_rights() == 0xff){
                int result = ((int (*)(int, char **))apps_s[temp_pointer].address)(sizeof(argv), argv);
                if (result == -1) printf("\n[ERROR] An error has occurred in the application!\nExit code: %d\n", result);

                return result;
            }
            temp_pointer++;
        }
        return 0x44f9ad;
    }
}