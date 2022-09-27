project('FireflyOS', ['cpp', 'c'], default_options: ['cpp_std=c++2a', 'b_asneeded=false', 'b_lundef=false', 'buildtype=plain'])

if not meson.is_cross_build()
    error('This is not a cross build, refer to the build instructions.')
endif

if meson.get_compiler('cpp').get_id() != 'gcc'
    error('Attempted to build with a compiler that is not gcc')
endif

# Set by subdirs
c_files   = []
cxx_files = []
asm_files = []
include_dir = include_directories('include/', 'include/cxxshim/', 'include/frigg/')

subdir('include/')
subdir('firefly/') # kernel

nasm = find_program('nasm')
asm_gen = generator(nasm, output: '@BASENAME@.o', arguments: ['-felf64', '@INPUT@', '-g', '-F', 'dwarf', '-o', '@OUTPUT@'])
asm_obj = asm_gen.process(asm_files)

executable_kwargs = {
	'cpp_args': kernel_cxx_flags,
	'c_args': kernel_c_flags,
	'include_directories': include_dir,
	'link_args': ['-Wl,-T../linkage/linker_x86_64.ld', '-nostdlib', 'kernel_x86_64.elf.p/symtable.o','../fonts/vgafont.obj', '-static', '-Wl,-z-max-page-size=0x1000'],
}

# Todo: symbol table should be a kernel module rather than re-linking the kernel
run_command('scripts/gensym.sh', check: true)

exe = executable(
    'kernel_x86_64.elf',
    c_files, cxx_files, asm_obj,
    kwargs: executable_kwargs
)

custom_target(
    'Relink',
    depends: exe,
    input: exe,
    output: 'Target Relink failed',
    command: ['python3', '../scripts/symbol_table_x86_64.py', 'kernel_x86_64.elf'],
    build_by_default: true
)
