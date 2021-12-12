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

#include <x86_64/api/api_config.hpp>
#include <x86_64/api/std/cstring.h>

#define BLOCK_LIMIT 1024
#define BLOCK_SIZE_LIMIT 8192

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

    memory_block *(*b[2])(uint32_tt) = {};
    memory_block *(*c[2])(uint64_tt, uint32_tt) = {};

    uint32_tt (*d[2])(void) = {};

    void (*e[2])(char) = {};

    uint8_tt (*f[5])(void) = {}; //unused

    uint32_tt (*g[2])(const char *) = {};

    void (*h[2])(unsigned long long) = {};

    void (*i[2])(const char *, api_size_t, bool) = {};

    void (*j[2])(const char *) = {};

    int (*k[2])(uint32_tt) = {};

    char *(*l[2])(api_size_t, char *, int) = {};

    char *(*m[2])(api_size_t, char *, int, bool) = {};

    void (*n[2])(void) = {};
    
    /*
        Kernel Settings:
            1.   - Kernel Mode ('d')debug, ('p')production
            2.   - Enable Serial Port ('y')yes, ('n')no
            3.   - Disable Application Access Rights ('y')yes, ('n')no
            4.   - Disable Memory Block Access Rights ('y')yes, ('n')no
            5.   - Limit Block Count to ('h')512 or ('l')256 or ('f')1024
            6.   - Print interrupts and exceptions on serial port too. ('y')yes, ('n')no
            7-8. - Reserved
    */
    unsigned char *kernel_settings;
    char *kernel_settings_raw;
};

namespace FAPI {
    namespace std {
        /** 
         * @param uint32_t num
         * @return int
         **/
        int digitcount(uint32_tt num);

        /** 
         * @param size_t num
         * @param char* str
         * @param int base
         * @return char *
         **/
        char *itoa(api_size_t num, char *str, int base);

        /** 
         * @param size_t num
         * @param char* str
         * @param int base
         * @param bool toupper
         * @return char *
         **/
        char *itoa(api_size_t num, char *str, int base, bool toupper);
    }
    namespace stdio {
        /** 
         * @param string text (const char *, ...)
         * @return int
         **/
        int printf(const char *text, ...);
    }
    namespace kernel {
        namespace sleep {
            /** 
            * @param unsigned_long_long mss
            * @return void
            **/
            void sleep(unsigned long long mss);
        }
        namespace checksum {
            /** 
            * @param string *text (cosnt char *)
            * @return uint32_t
            **/
            uint32_tt checksum(const char *text);
        }
        namespace settings {
            namespace get {
                /** 
                * @return uint8_t
                **/
                uint8_tt block_count ();

                /** 
                * @return uint8_t
                **/
                uint8_tt disable_app_access_rights ();

                /** 
                * @return uint8_t
                **/
                uint8_tt disable_memory_block_access_rights ();

                /** 
                * @return uint8_t
                **/
                uint8_tt enable_serial_port ();

                /** 
                * @return uint8_t
                **/
                uint8_tt kernel_mode ();
            }
        }
        namespace drivers {
            namespace vbe {
                /** 
                * @param char c
                * @return void
                **/
                void putc (char c);
                
                /** 
                * @param string *text (const char *)
                * @return void
                **/
                void puts(const char *text);
            }
        }
        namespace io {
            namespace legacy {
                /**
                 * @param string *text (const char *)
                 * @param ... (not supported by API. Use print_format)
                 * @return int
                 **/
                int writeTextSerial (const char *, ...);

                /** 
                * @param char c
                * @return void
                 **/
                void writeCharSerial (char c);
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
    namespace format {
        /**
         * For formatted string you can use this function.
         * . 0x00 - print to vbe | 0x01 - print to serial port
         * @param unsigned_char device_to_write
         * @param string *fmt (const char *)
         * @param ... additional arguments
         * 
        **/
        int print_format(unsigned char device_to_write, const char* fmt, ...);
    }
}