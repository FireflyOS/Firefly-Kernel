/* api.h
 *   by SergeyMC9730
 *
 * Created:
 *   9/26/2021, 7:45:07 PM
 * Last edited:
 *   9/26/2021, 8:05:01 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This is Firefly API for external applications. 
**/

#pragma once

#define BLOCK_LIMIT 1024
#define BLOCK_SIZE_LIMIT 8192

#define FAPI_PRINTF               0xFFFFFFFF802055D0
#define FAPI_WRITETEXTSERIAL      0xFFFFFFFF802035D0
#define FAPI_USE_BLOCK            0xFFFFFFFF80200FE0
#define FAPI_GET_BLOCK            0xFFFFFFFF80201130
#define FAPI_GET_BLOCK_LIMIT      0xFFFFFFFF802011D0
#define FAPI_GET_BLOCK_SIZE_LIMIT 0xFFFFFFFF802011E0

namespace FAPI{ 
    namespace stdio {
        /** 
         * @param string (const char *, ...)
         * @return int
         **/
        int printf(const char *text, ...);
    }
    namespace kernel {
        namespace io {
            namespace legacy {
                /**
                 * @param string (const char *, ...)
                 * @return void
                 **/
                void fapi_writeTextSerial (const char *, ...);
            }
        }
    }
    namespace mm {
        namespace greanleafy {
            struct memory_block {
                unsigned char block[BLOCK_SIZE_LIMIT];
                unsigned int  block_access;
                unsigned long block_number;
                unsigned char is_used;
            }; //(BLOCK_SIZE_LIMIT + 14) bytes per block 

            /** 
             * @param uint64_t block_number
             * @param uint32_t access
             * @return void * (memory_block struct)
             **/
            memory_block fapi_get_block (unsigned long, unsigned int);

            /** 
             * @param uint32_t access
             * @return memory_block struct
             **/
            memory_block use_block (unsigned int access);

            /**
             * @return uint32_t
             **/
            unsigned int fapi_get_block_limit (void);

            /**
             * @return uint32_t
             **/
            unsigned int fapi_get_block_size_limit (void);
        }
    }
}