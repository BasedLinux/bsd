project('bsd-util-c', 'cpp',
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

configdata = configuration_data()

deps_private_maybe_subproject = [
  dependency('bsd-util'),
]
deps_public_maybe_subproject = [
]
subdir('bsd-meson-build-support/subprojects')

configdata.set_quoted('PACKAGE_VERSION', meson.project_version())

config_priv_h = configure_file(
  configuration : configdata,
  output : 'bsd_api_util_config.h',
)

subdir('bsd-meson-build-support/common')

sources = files(
  'bsd_api_util.cc',
)

include_dirs = [include_directories('.')]

headers = files(
  'bsd_api_util.h',
)

# TODO don't install this once tests don't use it.
headers += files('bsd_api_util_internal.h')

subdir('bsd-meson-build-support/export-all-symbols')
subdir('bsd-meson-build-support/windows-version')

this_library = library(
  'bsdutilc',
  sources,
  config_priv_h,
  dependencies : deps_public + deps_private + deps_other,
  include_directories : include_dirs,
  link_args: linker_export_flags,
  prelink : true, # For C++ static initializers
  install : true,
)

install_headers(headers, preserve_path : true)

libraries_private = []

subdir('bsd-meson-build-support/export')
