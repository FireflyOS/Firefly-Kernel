#include <stl/cstdlib/stdio.h>

#include <i386/trace/symbols.hpp>
#include <stl/cstdlib/cstring.h>

extern "C" sym_table_t symbol_table[];

sym_table_t SymbolTable::operator[](uint32_t addr) const noexcept {
    return lookup(addr);
}

//Parse the symbol table and find the correct address of EIP
sym_table_t SymbolTable::lookup(uint32_t addr) const noexcept {
    //We need the largest address that is smaller than `addr` because of the jsr instruction, otherwise the addresses are off
    uint32_t corrected_address = 0;
    uint32_t index_new = 0;

    for (int i = 0;; i++) {
        uint32_t sym_addr = symbol_table[i].addr;
        if (corrected_address >= addr)
            continue;

        else if (sym_addr < addr) {
            if (sym_addr > corrected_address) {
                corrected_address = sym_addr;
                index_new = i;
            }
        }

        if (sym_addr == 0xFFFFFFFF) {
            return { corrected_address, symbol_table[index_new].name };
        }
    }
}

bool backtrace(uint32_t addr) {
    SymbolTable table{};
    auto const&[base, name] = table[addr];
    printf("\t%s\n", name);

    /* Don't trace symbols below kernel_main */
    if (strcmp(name, "kernel_main") == 0)
        return false;
    
    return true;
}