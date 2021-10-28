#include <x86_64/sleep.hpp>

namespace firefly::kernel::sleep {
    void sleep(unsigned long long mss){
        unsigned long long i = 0;
        while(i < (mss * 100)){
            i++;
        }
        return;
    }
}