#include <stl/cstdlib/stdio.h>
#include <stl/cstdlib/cstdint.h>

namespace firefly::applications::test {
    int test_main(uint8_t *virtual_memory_instance){
        printf("Hello World!\n0: %d\n\n", virtual_memory_instance[0]);
        return 0;
    }
}