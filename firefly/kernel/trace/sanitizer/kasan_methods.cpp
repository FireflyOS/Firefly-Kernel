#include "firefly/logger.hpp"
#include "firefly/memory-manager/virtual/virtual.hpp"
#include "firefly/trace/sanitizer/kasan.hpp"
#include "libk++/bits.h"

extern "C" void __asan_loadN_noabort(void* p, size_t size) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), size, false);
}

extern "C" void __asan_storeN_noabort(void* p, int size) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), size, true);
}

extern "C" void __asan_load1_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 1, false);
}
extern "C" void __asan_load2_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 2, false);
}
extern "C" void __asan_load4_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 4, false);
}
extern "C" void __asan_load8_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 8, false);
}
extern "C" void __asan_load16_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 16, false);
}
extern "C" void __asan_store1_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 1, true);
}

extern "C" void __asan_store2_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 2, true);
}

extern "C" void __asan_store4_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 4, true);
}

extern "C" void __asan_store8_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 8, true);
}
extern "C" void __asan_store16_noabort(void* p) {
    firefly::kernel::kasan::verifyAccess(reinterpret_cast<uintptr_t>(p), 16, true);
}

extern "C" void __asan_handle_no_return() {
    // unused
}

extern "C" void __asan_before_dynamic_init(const char* module_name) {
    // unused
}

extern "C" void __asan_after_dynamic_init() {
    // unused
}