#include <x86_64/random.hpp>

namespace firefly::kernel::randomizer {
    unsigned long int next = 1;
    int rand(){
        next = next * 0x41c64e76 + 12345;
        return (unsigned int)(next / 65536) % 32768;
    }

    void srand(unsigned int seed){
        next = seed;
    }
}