#pragma once

#include <stdint.h>

typedef struct
{
    uint32_t addr;
    const char *name;
} sym_table_t;

class SymbolTable {
public:
    sym_table_t lookup(uint32_t) const noexcept;
    sym_table_t operator[](uint32_t) const noexcept;
};

bool backtrace(uint32_t addr);