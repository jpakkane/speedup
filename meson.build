project('speedup test', 'cpp',
  default_options : ['cpp_std=c++11',
  'buildtype=debugoptimized'])

# This one has only portable C++ code, no asm,
# no intrinsics, no dependencies.
executable('speedup', 'main.cpp', 'speedup.cpp')

# This one can use intrinsics & stuff.
# Requires AVX2.
if host_machine.cpu_family() == 'x86_64'
  avx_args = '-mavx2' # FIXME make work on MSVC.
  executable('speedup_x86', 'main_x86.cpp', 'speedup_x86.cpp', cpp_args : avx_args)
endif
