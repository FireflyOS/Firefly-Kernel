#include "firefly/memory-manager/primary/primary_phys.hpp"
#include <cstddef>

/*
    All kinds of stubs for functions that prevented a successful linking process reside here.
*/
extern "C" int __cxa_atexit([[maybe_unused]] void (*func)(void *), [[maybe_unused]] void *arg, [[maybe_unused]] void *dso_handle) {
    return 0;
}

extern "C" void *alloc_mem(size_t size)
{
	return firefly::kernel::mm::Physical::must_allocate(size);
}

extern "C" void free_mem(void *ptr, [[maybe_unused]] size_t size) {
	firefly::kernel::mm::Physical::deallocate(ptr);
}