#include <x86_64/random.hpp>
#include <stl/cstdlib/cstdint.h>
#include <stl/cstdlib/stdio.h>

namespace firefly::kernel::randomizer {
    namespace experimental {
        [[maybe_unused]] static inline bool test(){
            return true;
        }
    }
    unsigned long int next = 1;
    int rand(){
        next = next * 0x41c64e76 + 12345;

        return (unsigned int)(next / 65536) % 32768;
    }

    void srand(unsigned int seed){
        next = seed;
    }
}