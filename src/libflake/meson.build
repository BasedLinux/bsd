project('bsd-flake', 'cpp',
  version : files('.version'),
  default_options : [
    'cpp_std=c++2a',
    # TODO(Qyriad): increase the warning level
    'warning_level=1',
    'errorlogs=true', # Please print logs for tests that fail
  ],
  meson_version : '>= 1.1',
  license : 'LGPL-2.1-or-later',
)

cxx = meson.get_compiler('cpp')

subdir('bsd-meson-build-support/deps-lists')

deps_private_maybe_subproject = [
]
deps_public_maybe_subproject = [
  dependency('bsd-util'),
  dependency('bsd-store'),
  dependency('bsd-fetchers'),
  dependency('bsd-expr'),
]
subdir('bsd-meson-build-support/subprojects')

nlohmann_json = dependency('nlohmann_json', version : '>= 3.9')
deps_public += nlohmann_json

subdir('bsd-meson-build-support/common')

subdir('bsd-meson-build-support/generate-header')

generated_headers = []
foreach header : [
  'call-flake.nix',
]
  generated_headers += gen_header.process(header)
endforeach

sources = files(
  'config.cc',
  'flake.cc',
  'flakeref.cc',
  'lockfile.cc',
  'flake-primops.cc',
  'settings.cc',
  'url-name.cc',
)

subdir('include/bsd/flake')

subdir('bsd-meson-build-support/export-all-symbols')
subdir('bsd-meson-build-support/windows-version')

this_library = library(
  'bsdflake',
  sources,
  generated_headers,
  dependencies : deps_public + deps_private + deps_other,
  include_directories : include_dirs,
  link_args: linker_export_flags,
  prelink : true, # For C++ static initializers
  install : true,
)

install_headers(headers, subdir : 'bsd/flake', preserve_path : true)

libraries_private = []

subdir('bsd-meson-build-support/export')
