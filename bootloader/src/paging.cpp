#include <err.hpp>
#include <paging.hpp>

void setup_pages(unsigned long kernel_size) {
    asm volatile(
        "movw %%ax, %%ds"
        :
        : "a"(0x2000));

    unsigned long long *pml4 = reinterpret_cast<unsigned long long *>(0x0000);
    unsigned long long *pdpt = reinterpret_cast<unsigned long long *>(0x1000);
    unsigned long long *pd = reinterpret_cast<unsigned long long *>(0x2000);

    unsigned long kernel_end = 0x00100000 + kernel_size;

    // identity map to lower and higher virtual address space
    pml4[0] = 0x00021003ull;

    // { true,
    //                                           true,
    //                                           false,
    //                                           false,
    //                                           false,
    //                                           false,
    //                                           0,
    //                                           0,
    //                                           0,
    //                                           0x00020000ull | (reinterpret_cast<unsigned long long>(pdpt) >> 12),
    //                                           0 };
    pdpt[0] = pdpt[510] = 0x00022003ull;

    // { true,
    //                                           true,
    //                                           false,
    //                                           false,
    //                                           false,
    //                                           false,
    //                                           0,
    //                                           0,
    //                                           0,
    //                                           0x00020000ull | (reinterpret_cast<unsigned long long>(pd) >> 12),
    //                                           0 };

    unsigned long target = 0;
    // page table
    unsigned long long pt_start = 0x3000;

    // Iterate until we've mapped all of the kernel, or until we've filled the entire 64K segment with tables
    // which would allow for a 26MiB kernel.
    for (unsigned int pd_entry = 0; target < kernel_end && pt_start < 0xf000; pd_entry++, pt_start += 0x1000) {
        pd[pd_entry] = 0x0002003ull | pt_start;

        // { true,
        //                  true,
        //                  false,
        //                  false,
        //                  false,
        //                  false,
        //                  0,
        //                  0,
        //                  0,
        //                  0x00020000ull | (pt_start >> 12),
        //                  0 };

        unsigned long long *pt = reinterpret_cast<unsigned long long *>(pt_start);

        for (int pt_entry = 0; pt_entry < 512 && target < kernel_end; pt_entry++, target += 4096) {
            pt[pt_entry] = 0x00020003ull | target;

            //{ true, true, false, false, false, false, false, false, 0, target >> 12, 0 };
        }
    }

    // already used last 4K table and still not mapped all of kernel
    if (pt_start == 0xf000 && target < kernel_end)
        err();

    asm volatile(
        "mov %%ax, %%ds"
        :
        : "a"(0x0000));
}