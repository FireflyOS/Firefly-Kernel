#include <cstddef>
#include <frg/macros.hpp>

#include "cstdlib/cassert.h"
#include "firefly/memory-manager/primary/primary_phys.hpp"
#include "firefly/memory-manager/secondary/heap.hpp"
#include "firefly/panic.hpp"

/*
    All kinds of stubs for functions that prevented a successful linking process reside here.
*/
extern "C" {
int __cxa_atexit([[maybe_unused]] void (*func)(void *), [[maybe_unused]] void *arg, [[maybe_unused]] void *dso_handle) {
    return 0;
}

void *alloc_mem(size_t size) {
    return firefly::kernel::mm::Physical::must_allocate(size);
}

void free_mem(void *ptr, [[maybe_unused]] size_t size) {
    firefly::kernel::mm::Physical::deallocate(ptr);
}

void __stack_chk_fail(void) {
}

void FRG_INTF(log)(const char *cstring) {
}

void FRG_INTF(panic)(const char *cstring) {
    firefly::panic(cstring);
}
}

void *operator new(size_t sz) {
    return firefly::kernel::mm::heap->allocate(sz);
}

void operator delete(void *ptr, [[gnu::unused]] size_t size) {
    firefly::kernel::mm::heap->deallocate(ptr);
}
