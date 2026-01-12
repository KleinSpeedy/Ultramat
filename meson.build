project('Ultramat', 'c',
  default_options : ['warning_level=3', 'c_std=c11'])

# source file list
source_files=[]
subdir('src')

include_dirs=[
  include_directories('inc'),
]

# retrieve app version information from git tag
git = find_program('git', required: true)
git_res = run_command(git, 'describe', '--tags', '--dirty', '--match=v*', check: true)

vers_str = git_res.stdout().strip().split('-')
# get major, minor, bugfix, provide fallback version
tag = vers_str.get(0, 'v0.0.1').strip('v').split('.')
# configure version header file
vers_cfg = configuration_data()
vers_cfg.set('version_str', '-'.join(vers_str.get(0), vers_str.get(2)))
vers_cfg.set('version_major', tag.get(0).to_int())
vers_cfg.set('version_minor', tag.get(1).to_int())
vers_cfg.set('version_bugfix', tag.get(2).to_int())
configure_file(input : files('scripts/ultra_version.h.in'),
               output : 'ultra_version.h',
               configuration : vers_cfg)

# Configure c-periphery cmake project
cmake = import('cmake')

c_periphery_opts = cmake.subproject_options()
c_periphery_opts.add_cmake_defines({'BUILD_TESTS': false})
c_periphery_opts.set_override_option('c_std', 'gnu99')

c_periphery_proj = cmake.subproject('c-periphery', options: c_periphery_opts)
c_periphery_dep = c_periphery_proj.dependency('periphery')

nanopb_proj = subproject('nanopb')
nanopb_dep = nanopb_proj.get_variable('nanopb_dep')
generator_path = nanopb_proj.get_variable('generator_path')

nanopb_generator = generator(find_program(generator_path[0]),
    arguments: [ generator_path[1], '-I', '@CURRENT_SOURCE_DIR@', '-D', '@BUILD_DIR@', '--strip-path', '@INPUT@' ],
    output: [ '@BASENAME@.pb.c', '@BASENAME@.pb.h' ]
)

generated_proto = nanopb_generator.process(files(
    'proto/commands.proto'
), preserve_path_from: meson.current_source_dir())

source_files += generated_proto

# Math library
cc = meson.get_compiler('c')
math_dep = cc.find_library('m', required: true)

# Dependencies
deps = [
  dependency('gtk+-3.0'),
  dependency('glib-2.0'),
  dependency('gobject-2.0'),
  math_dep,
  c_periphery_dep,
  nanopb_dep,
]

suffix = build_machine.cpu_family()

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
