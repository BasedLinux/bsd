project('bsd-store-c', 'cpp',
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
  dependency('bsd-util'),
  dependency('bsd-store'),
]
deps_public_maybe_subproject = [
  dependency('bsd-util-c'),
]
subdir('bsd-meson-build-support/subprojects')

subdir('bsd-meson-build-support/common')

sources = files(
  'bsd_api_store.cc',
)

include_dirs = [include_directories('.')]

headers = files(
  'bsd_api_store.h',
)

# TODO don't install this once tests don't use it and/or move the header into `libstore`, non-`c`
headers += files('bsd_api_store_internal.h')

subdir('bsd-meson-build-support/export-all-symbols')
subdir('bsd-meson-build-support/windows-version')

this_library = library(
  'bsdstorec',
  sources,
  dependencies : deps_public + deps_private + deps_other,
  include_directories : include_dirs,
  link_args: linker_export_flags,
  prelink : true, # For C++ static initializers
  install : true,
)

install_headers(headers, preserve_path : true)

libraries_private = []

subdir('bsd-meson-build-support/export')
