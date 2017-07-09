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

if get_option('use_compute')
  conf = configuration_data()
  conf.set('BOOST_COMPUTE_DEBUG_KERNEL_COMPILATION', get_option('buildtype').contains('debug'))
  configure_file(output : 'config.h', configuration : conf)

  if host_machine.system() == 'darwin'
    link_opencl_flags = ['-framework', 'OpenCL']
  else
    link_opencl_flags = '-lOpenCL'
  endif

  # OpenCL doesn't do pkg-config or have Meson support
  executable('speedup_compute', 'main_compute.cpp', 'speedup_compute.cpp',
             link_args: link_opencl_flags)
endif

