/* api.c
 *   by SergeyMC9730
 *
 * Created:
 *   9/26/2021, 7:45:07 PM
 * Last edited:
 *   9/27/2021, 5:54:53 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This is Firefly API for external applications.
**/

#include <x86_64/api/api.hpp>

namespace FAPI {
    fp *get_fp(void){
        return ((fp *(*)(void))(fp *)FAPI_ENDP)();
    }
    namespace stdio {
        int printf(const char *text, ...){
            auto *sys_struct = (fp *)get_fp();
            int result = ((int (*)(const char *, ...))sys_struct->a[0])(text);
            return result;
        } 
    }
    namespace kernel {
        namespace io {
            namespace legacy {
                int writeTextSerial (const char *text, ...) {
                    auto *sys_struct = (fp *)get_fp();
                    int result = ((int (*)(const char *, ...))sys_struct->a[1])(text);
                    return result;
                }
            }
        }
    }
    namespace mm {
        namespace greanleafy {
            memory_block *get_block (uint64_tt block_number, uint32_tt access) {
                auto *sys_struct = (fp *)get_fp();
                return ((memory_block *(*)(uint64_tt, uint32_tt))sys_struct->c[0])(block_number, access);
            }

            memory_block *use_block(uint32_tt access) {
                auto *sys_struct = (fp *)get_fp();
                return ((memory_block *(*)(uint32_tt))sys_struct->b[0])(access);
            }

            uint32_tt get_block_limit (void) {
                auto *sys_struct = (fp *)get_fp();
                int result = ((uint32_tt (*)(void))sys_struct->d[0])();
                return result;
            }

            uint32_tt get_block_size_limit (void) {
                auto *sys_struct = (fp *)get_fp();
                int result = ((uint32_tt (*)(void))sys_struct->d[1])();
                return result;
            }
        }
    }
}