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

c_periphery_proj = cmake.subproject('c-periphery', options: c_periphery_opts)
c_periphery_dep = c_periphery_proj.dependency('periphery')

# Dependencies
deps = [
  dependency('gtk+-3.0'),
  dependency('glib-2.0'),
  dependency('gobject-2.0'),
  c_periphery_dep
]

# Enable debugging and warnings for development builds
if get_option('buildtype') == 'debug'
  add_project_arguments('-DGTK_DEBUG=interactive', '-DG_DEBUG=fatal-warnings',
                        language: 'c')
endif

# Release target
build_target(meson.project_name(),
  source_files,
  include_directories: include_dirs,
  dependencies: deps,
  override_options: [
    'buildtype=release'
  ],
target_type: 'executable')

# Debug target
build_target(meson.project_name() + '_dbg',
  source_files,
  include_directories: include_dirs,
  dependencies: deps,
  override_options: [
    'buildtype=debug'
  ],
target_type: 'executable')
