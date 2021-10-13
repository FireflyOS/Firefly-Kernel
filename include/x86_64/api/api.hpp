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

// FAPI_PRINTF               0 //int (const char *text, ...) 0 a
// FAPI_WRITETEXTSERIAL      0 //int (const char *text, ...) 1 a
// FAPI_USE_BLOCK            0 //memory_block * (uint32_tt access) 0 b
// FAPI_GET_BLOCK            0 //memory_block * (uint64_tt block_number, uint32_tt access) c
// FAPI_GET_BLOCK_LIMIT      0 //uint32_tt (void) 0 d
// FAPI_GET_BLOCK_SIZE_LIMIT 0 //uint32_tt (void) 1 d

#define FAPI_ENDP 0xFFFFFFFF802007B0

#define uint8_tt  unsigned char
#define uint32_tt unsigned int
#define uint64_tt unsigned long

struct memory_block {
    uint8_tt  block[BLOCK_SIZE_LIMIT];
    uint32_tt block_access;
    uint64_tt block_number;
    uint8_tt  is_used;
}; //(BLOCK_SIZE_LIMIT + 14) bytes per block

struct fp {
    int (*a[2])(const char *, ...) = {};

    memory_block *(*b[2])(uint32_tt access) = {};
    memory_block *(*c[2])(uint64_tt block_number, uint32_tt access) = {};

    uint32_tt (*d[2])(void) = {};
};

namespace FAPI {
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
                int writeTextSerial (const char *, ...);
            }
        }
    }
    namespace mm {
        namespace greanleafy {
            /** 
             * @param uint64_tt block_number
             * @param uint32_tt access
             * @return void * (memory_block struct)
             **/
            memory_block *get_block (uint64_tt block_number, uint32_tt access);

            /** 
             * @param uint32_tt access
             * @return memory_block struct
             **/
            memory_block *use_block (uint32_tt access);

            /**
             * @return uint32_tt
             **/
            uint32_tt get_block_limit (void);

            /**
             * @return uint32_tt
             **/
            uint32_tt get_block_size_limit (void);
        }
    }
}