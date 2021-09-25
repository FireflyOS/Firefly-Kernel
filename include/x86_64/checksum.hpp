#pragma once

namespace firefly::kernel::checksum {
    /**
     * This function uses char * to get checksum
     * 
     * @param string char *string
     * 
     * @return Checksum of a string (unsigned int) 
     */
    unsigned int checksum(const char *string);
}