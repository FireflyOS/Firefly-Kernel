#include "firefly/trace/symbols.hpp"

#include <stl/cstdlib/cstring.h>
#include <stl/cstdlib/stdio.h>

#include "firefly/logger.hpp"

extern "C" sym_table_t symbol_table[];

sym_table_t SymbolTable::operator[](uint64_t addr) const noexcept {
    return lookup(addr);
}

//Parse the symbol table and find the correct address of RIP
sym_table_t SymbolTable::lookup(uint64_t addr) const noexcept {
    //We need the largest address that is smaller than `addr` because of the jsr instruction, otherwise the addresses are off
    uint64_t corrected_address = 0;
    uint64_t index_new = 0;

    for (int i = 0;; i++) {
        uint64_t sym_addr = symbol_table[i].addr;
        if (corrected_address >= addr)
            continue;

        else if (sym_addr < addr) {
            if (sym_addr > corrected_address) {
                corrected_address = sym_addr;
                index_new = i;
            }
        }

        if (sym_addr == 0xFFFFFFFFFFFFFFFF) {
            return { corrected_address, symbol_table[index_new].name };
        }
    }
}

bool backtrace([[maybe_unused]]uint64_t addr, [[maybe_unused]]int iteration) {
    SymbolTable table{};
    [[maybe_unused]]auto const& [base, name] = table[addr];


    using firefly::kernel::info_logger;
    info_logger << "#" << iteration << " "  << info_logger.hex(base) << " \t" << name << '\n';

    /* Don't trace symbols below kernel_init */
    if (strcmp(name, "kernel_init") == 0)
        return false;

    return true;
}