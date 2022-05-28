from os import popen, system
import sys

PARSED =  "parsed_x86_64.sym"
KERNEL =  sys.argv[1]
writer = open(PARSED, "w+")

symbol_table_naked = """
#include <symbols.hpp>

sym_table_t symbol_table[] = {
"""

def init_writer():
    writer.seek(0)
    writer.truncate()
    writer.write(symbol_table_naked)

# return an entry in the symbol array: {0xdeadbeef, "function"},
def symbol_template(address, function_name):
    return '\t{0x' + address + ', ' + '\"' + function_name + '\"' + '},\n'

def write_data(address, function_name):
    if function_name == "":
        return
    writer.write(symbol_template(address=address, function_name=function_name))


def destroy_writer():
    writer.write("{0xFFFFFFFFFFFFFFFF, \"\"}")
    writer.write("};")
    writer.close()

# Input: A single line from objdump, returns true if it's a function, otherwise it returns false since we don't need variables or function names
def is_function_symbol(objdump_line):
    if not ".text" in objdump_line:
        return False
    return True

def function_symbol_extract(objdump_line):
    objdump_line = str(objdump_line).split()
    return (str(objdump_line[0]), str(objdump_line[5:]).replace("]", "").replace("'","").replace("[", "").replace(",,", ","))

def parse_symbol_tables(unparsed_sym_table):
    unparsed_sym_table = unparsed_sym_table.splitlines()
    
    for x in range(len(unparsed_sym_table)-1):
        if not is_function_symbol(unparsed_sym_table[x]):
            continue
        else:
            addr, name = function_symbol_extract(unparsed_sym_table[x])
            write_data(str(addr), name)
        

def compile_sym_table():
    system(f"clang++ -ffreestanding -x c++ -c ./parsed_x86_64.sym -m64 -I ../include/firefly/trace -o kernel_x86_64.elf.p/symtable.o")

if __name__ == '__main__':
    # This might seem really stupid, and that's because it is
    # But for some reason the script just won't write the symbol
    # table correctly the first time (These issues started occuring after switching from make to meson)
    # I have no idea what the issue is, and I don't care enough to find an actual solution
    # If you figure it out, please make a PR
    for x in range(2):
        init_writer()
        parse_symbol_tables(popen(f"objdump -C -t {KERNEL}").read())
        destroy_writer()
        
        compile_sym_table()
        system('ld.lld -o ./kernel_x86_64.elf --no-undefined -T ../linkage/linker_x86_64.ld -nostdlib -m elf_x86_64 $(find ./ -name "*.o" -type f)') # Relink kernel with symbol tables
        writer = open(PARSED, "w+")

    print("[*] Wrote symbol table")
    print("[*] Compiled symbol table")
    print('[*] Relinked kernel')