project('bsd-expr-tests', 'cpp',
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
  dependency('bsd-expr'),
  dependency('bsd-expr-c'),
  dependency('bsd-expr-test-support'),
]
deps_public_maybe_subproject = [
]
subdir('bsd-meson-build-support/subprojects')

subdir('bsd-meson-build-support/export-all-symbols')
subdir('bsd-meson-build-support/windows-version')

rapidcheck = dependency('rapidcheck')
deps_private += rapidcheck

gtest = dependency('gtest')
deps_private += gtest

gmock = dependency('gmock')
deps_private += gmock

configdata = configuration_data()
configdata.set_quoted('PACKAGE_VERSION', meson.project_version())

config_priv_h = configure_file(
  configuration : configdata,
  output : 'expr-tests-config.hh',
)

subdir('bsd-meson-build-support/common')

sources = files(
  'derived-path.cc',
  'error_traces.cc',
  'eval.cc',
  'json.cc',
  'main.cc',
  'bsd_api_expr.cc',
  'bsd_api_external.cc',
  'bsd_api_value.cc',
  'primops.cc',
  'search-path.cc',
  'trivial.cc',
  'value/context.cc',
  'value/print.cc',
  'value/value.cc',
)

include_dirs = [include_directories('.')]


this_exe = executable(
  meson.project_name(),
  sources,
  config_priv_h,
  dependencies : deps_private_subproject + deps_private + deps_other,
  include_directories : include_dirs,
  # TODO: -lrapidcheck, see ../libutil-support/build.meson
  link_args: linker_export_flags + ['-lrapidcheck'],
  install : true,
)

test(
  meson.project_name(),
  this_exe,
  env : {
    '_NIX_TEST_UNIT_DATA': meson.current_source_dir() / 'data',
  },
  protocol : 'gtest',
)
