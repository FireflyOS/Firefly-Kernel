//i wrote this a long time ago...
namespace reglib {
    namespace write {
        inline void eax(int input){
            int joe=1234, fred;
            __asm__(
                "mov %1, %%eax\n" 
                "mov %1, %%eax\n"
                "add $2, %%eax\n"
                "mov %%eax, %0\n"
                :"=r" (fred) /* %0: Output variable list */
                :"r" (joe) /* %1: Input variable list */
                :"%eax" /* Overwritten registers ("Clobber list") */
            );
        }
    }
}