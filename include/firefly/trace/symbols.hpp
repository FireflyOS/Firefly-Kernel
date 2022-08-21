#pragma once

#include <stdint.h>

struct SymbolTablePair {
    uint64_t addr;
    const char *name;
};

class SymbolTable {
public:
    SymbolTablePair lookup(uint64_t) const noexcept;
    SymbolTablePair operator[](uint64_t) const noexcept;
};

bool backtrace(uint64_t addr, int iteration);