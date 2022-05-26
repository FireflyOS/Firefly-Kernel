#pragma once

#include <stdint.h>

typedef struct
{
    uint64_t addr;
    const char *name;
} sym_table_t;

class SymbolTable {
public:
    sym_table_t lookup(uint64_t) const noexcept;
    sym_table_t operator[](uint64_t) const noexcept;
};

bool backtrace(uint64_t addr, int iteration);