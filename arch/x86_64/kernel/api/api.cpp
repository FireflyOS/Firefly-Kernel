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
    void init(){
        return;
    }
    namespace stdio {
        int printf(const char *text, ...){
            int result = ((int (*)(const char *, ...))FAPI_PRINTF)(text);
            return result;
        } 
    }
    namespace kernel {
        namespace io {
            namespace legacy {
                void fapi_writeTextSerial (const char *text, ...) {
                    ((void (*)(const char *, ...))FAPI_WRITETEXTSERIAL)(text);
                }
            }
        }
    }
    namespace mm {
        namespace greanleafy {
            memory_block fapi_get_block (unsigned long block_number, unsigned int access) {
                return ((memory_block (*)(unsigned long, unsigned int))FAPI_GET_BLOCK)(block_number, access);
            }

            memory_block use_block(unsigned int access) {
                return ((memory_block (*)(unsigned int))FAPI_USE_BLOCK)(access);
            }

            unsigned int fapi_get_block_limit (void) {
                int result = ((unsigned int (*)(void))FAPI_GET_BLOCK_LIMIT)();
                return result;
            }

            unsigned int fapi_get_block_size_limit (void) {
                int result = ((unsigned int (*)(void))FAPI_GET_BLOCK_SIZE_LIMIT)();
                return result;
            }
        }
    }
}