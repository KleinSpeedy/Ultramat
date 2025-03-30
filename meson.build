project('Ultramat', 'c',
  version : '0.2',
  default_options : ['warning_level=3', 'c_std=c11'])

# source file list
source_files=[]
subdir('src')

include_dirs=[
  include_directories('inc'),
]

# Configure c-periphery cmake project
cmake = import('cmake')

c_periphery_opts = cmake.subproject_options()
c_periphery_opts.add_cmake_defines({'BUILD_TESTS': false})
c_periphery_opts.set_override_option('c_std', 'gnu99')

c_periphery_proj = cmake.subproject('c-periphery', options: c_periphery_opts)
c_periphery_dep = c_periphery_proj.dependency('periphery')

# Math library
cc = meson.get_compiler('c')
math_dep = cc.find_library('m', required: true)

# Dependencies
deps = [
  dependency('gtk+-3.0'),
  dependency('glib-2.0'),
  dependency('gobject-2.0'),
  math_dep,
  c_periphery_dep
]

suffix = 'x86_64'
if meson.is_cross_build()
  suffix = 'aarch64'
endif

# Release target
executable(meson.project_name(),
  source_files,
  include_directories: include_dirs,
  dependencies: deps,
  override_options: [
    'buildtype=release'
  ],
  name_suffix: suffix
)

# Debug target
executable(meson.project_name() + '_dbg',
  source_files,
  include_directories: include_dirs,
  dependencies: deps,
  override_options: [
    'buildtype=debug'
  ],
  name_suffix: suffix
)
