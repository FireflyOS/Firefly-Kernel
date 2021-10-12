#include <x86_64/sleep.hpp>

namespace firefly::kernel::sleep {
    void sleep(long mss){
        long i = 0;
        while(i < (mss * 10)){
            i++;
        }
        return;
    }
}