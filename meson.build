project('FireflyOS', 'cpp', default_options: ['cpp_std=c++20'])

if not meson.is_cross_build()
    error('This is not a cross build, refer to the build instructions.')
endif

if meson.get_compiler('cpp').get_id() != 'clang'
    error('Attempted to build with a compiler that is not clang')
endif

# Set by subdirs
cxx_files = []
asm_files = []
include_dir = include_directories('include/', 'include/stl/')

subdir('include/')
subdir('arch/x86_64/')

nasm = find_program('nasm')
asm_gen = generator(nasm, output: '@BASENAME@.o', arguments: ['-felf64', '@INPUT@', '-g', '-F', 'dwarf', '-o', '@OUTPUT@'])
asm_obj = asm_gen.process(asm_files)

executable_kwargs = {
    'cpp_args': kernel_build_flags,
    'include_directories': include_dir,
    'link_args': ['-Wl,-T../linkage/linker_x86_64.ld', '-nostdlib', 'kernel_x86_64.elf.p/symtable.o'],
    'link_depends': ['linkage/linker_x86_64.ld'],
    'pie': false # -fpie
}

# Todo: symbol table should be a kernel module rather than re-linking the kernel
run_command('Scripts/gensym.sh', check: true)

exe = executable(
    'kernel_x86_64.elf',
    cxx_files, asm_obj,
    kwargs: executable_kwargs
)

custom_target(
    'Relink',
    depends: exe,
    input: exe,
    output: 'Target Relink failed',
    command: ['python3', '../Scripts/symbol_table_x86_64.py', 'kernel_x86_64.elf'],
    build_by_default: true
)