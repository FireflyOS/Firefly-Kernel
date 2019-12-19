#include <fat32.hpp>

namespace fat32 {
    extern "C" vbr *const boot_vbr = reinterpret_cast<vbr *>(0xfe00);
}