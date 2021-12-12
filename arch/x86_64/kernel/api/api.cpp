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
#include <stdarg.h>

namespace FAPI {
    fp *get_fp(void){
        return ((fp *(*)(void))(fp *)FAPI_ENDP)();
    }
    namespace std {
        int digitcount(uint32_tt num){
            auto *sys_struct = (fp *)get_fp();
            int result = ((int (*)(uint32_tt))sys_struct->k[0])(num);
            return result;
        }
        
        char *itoa(api_size_t num, char *str, int base){
            auto *sys_struct = (fp *)get_fp();
            char *result = ((char *(*)(api_size_t, char *, int))sys_struct->l[0])(num, str, base);
            return result;
        }

        char *itoa(api_size_t num, char *str, int base, bool toupper){
            auto *sys_struct = (fp *)get_fp();
            char *result = ((char *(*)(api_size_t, char *, int, bool))sys_struct->m[0])(num, str, base, toupper);
            return result;
        }
    }
    namespace stdio {
        int printf(const char *text, ...){
            auto *sys_struct = (fp *)get_fp();
            int result = ((int (*)(const char *, ...))sys_struct->a[0])(text);
            return result;
        } 
    }
    namespace kernel {
        namespace sleep {
            void sleep(unsigned long long mss){
                auto *sys_struct = (fp *)get_fp();
                ((void (*)(unsigned long long))sys_struct->h[0])(mss);
                return;
            }
        }
        namespace checksum {
            uint32_tt checksum(const char *text){
                auto *sys_struct = (fp *)get_fp();
                return ((uint32_tt (*)(const char *))sys_struct->g[0])(text);
            }
        }
        namespace settings {
            unsigned char *kernel_settings(){
                auto *sys_struct = (fp *)get_fp();
                return sys_struct->kernel_settings;
            }
            char *instruction(){
                auto *sys_struct = (fp *)get_fp();
                return sys_struct->kernel_settings_raw;
            }
        }
        namespace drivers {
            namespace vbe {
                void putc (char c){
                    auto *sys_struct = (fp *)get_fp();
                    ((void (*)(char))sys_struct->e[0])(c);
                    return;
                }
                void puts(const char *text){
                    auto *sys_struct = (fp *)get_fp();
                    ((void (*)(const char *))sys_struct->j[0])(text);
                    return;
                }
            }
        }
        namespace io {
            namespace legacy {
                int writeTextSerial (const char *text, ...) {
                    auto *sys_struct = (fp *)get_fp();
                    int result = ((int (*)(const char *, ...))sys_struct->a[1])(text);
                    return result;
                }
                void writeCharSerial (char c) {
                    auto *sys_struct = (fp *)get_fp();
                    ((void (*)(char))sys_struct->e[1])(c);
                    return;
                }
                void writeSerial (const char *text, api_size_t size, bool istoupper) {
                    auto *sys_struct = (fp *)get_fp();
                    ((void (*)(const char *, api_size_t, bool))sys_struct->i[0])(text, size, istoupper);
                    return;
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

            memory_block *use_block (uint32_tt access) {
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
    namespace format {
        int print_format(unsigned char device_to_write, const char* fmt, ...) {
            //0x00 - vbe
            //0x01 - serial port
            va_list ap;
            va_start(ap, fmt);
            int i = 0;
            int len = api_strlen(fmt);
            int res = 0;

            for (; i < len; i++) {
                switch (fmt[i]) {
                    case '%': {
                        switch (fmt[i + 1]) {
                        case 'c': {
                            char arg = va_arg(ap, int);
                            (device_to_write == 0x00) ? kernel::drivers::vbe::putc(arg) : kernel::io::legacy::writeCharSerial(arg);
                            i += 2, ++res;
                            break;
                        }

                        case 's': {
                            char* arg = va_arg(ap, char*);
                            (device_to_write == 0x00) ? kernel::drivers::vbe::puts(arg) : kernel::io::legacy::writeSerial(arg, api_strlen(arg), false);
                            i += 2, (res += 2 + api_strlen(arg));
                            break;
                        }

                        case 'i':
                        case 'd': {
                            api_size_t arg = va_arg(ap, api_size_t);
                            if (arg == 0)
                                (device_to_write == 0x00) ? kernel::drivers::vbe::putc('0') : kernel::io::legacy::writeCharSerial('0');

                            else {
                                char buff[20];
                                (device_to_write == 0x00) ? kernel::drivers::vbe::puts(std::itoa(arg, buff, 10)) : kernel::io::legacy::writeSerial(std::itoa(arg, buff, 10), api_strlen(std::itoa(arg, buff, 10)), false);
                                res += std::digitcount(arg);
                            }
                            i += 2;
                            break;
                        }

                        case 'x': {
                            api_size_t arg = va_arg(ap, api_size_t);
                            char buff[20];
                            (device_to_write == 0x00) ? kernel::drivers::vbe::puts(std::itoa(arg, buff, 16)) : kernel::io::legacy::writeSerial(std::itoa(arg, buff, 16), api_strlen(std::itoa(arg, buff, 16)), false);
                            res += api_strlen(buff);
                            i += 2;
                            break;
                        }
                        case 'X': {
                            api_size_t arg = va_arg(ap, api_size_t);
                            char buff[20];
                            (device_to_write == 0x00) ? kernel::drivers::vbe::puts(std::itoa(arg, buff, 16, true)) : kernel::io::legacy::writeSerial(std::itoa(arg, buff, 16), api_strlen(std::itoa(arg, buff, 16)), true);
                            res += api_strlen(buff);
                            i += 2;
                            break;
                        }

                        case 'o': {
                            api_size_t arg = va_arg(ap, api_size_t);
                            char buff[20];
                            (device_to_write == 0x00) ? kernel::drivers::vbe::puts(std::itoa(arg, buff, 8)) : kernel::io::legacy::writeSerial(std::itoa(arg, buff, 8), api_strlen(std::itoa(arg, buff, 8)), false);
                            i += 2;
                            break;
                        }
                        default:
                            va_end(ap);
                            break;
                    }
            }
                    default:
                        (device_to_write == 0x00) ? kernel::drivers::vbe::putc(fmt[i]) : kernel::io::legacy::writeCharSerial(fmt[i]);
                        va_end(ap);
                        res++;
                        break;
                }
            }
            return 0;
        }
    }
}