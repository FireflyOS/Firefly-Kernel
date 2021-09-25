#include <x86_64/checksum.hpp>

namespace firefly::kernel::checksum {
    /**
     * This function uses char * to get checksum
     * 
     * @param string const char *string
     * 
     * @return Checksum of a string (int) 
     */
    int checksum(const char *string){
        const char *string_to_chksum;
        int result = 0;

        string_to_chksum = string;
        while(*string_to_chksum != '\0') result += (int)(*string_to_chksum++) - '0';

        return result;
    }
}