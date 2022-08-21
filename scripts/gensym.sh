#!/bin/bash

echo '#include <symbols.hpp>' > build/parsed_x86_64.sym
echo 'SymbolTablePair symbol_table[] = {{0xFFFFFFFFFFFFFFFF, ""}};' >> build/parsed_x86_64.sym
mkdir build/kernel_x86_64.elf.p/
clang++ -x c++ -I include/firefly/trace -m64 -c build/parsed_x86_64.sym -o build/kernel_x86_64.elf.p/symtable.o