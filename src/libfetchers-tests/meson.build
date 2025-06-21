project('bsd-fetchers-tests', 'cpp',
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
  dependency('bsd-store-test-support'),
  dependency('bsd-fetchers'),
  dependency('bsd-fetchers-c'),
]
deps_public_maybe_subproject = [
]
subdir('bsd-meson-build-support/subprojects')

subdir('bsd-meson-build-support/export-all-symbols')
subdir('bsd-meson-build-support/windows-version')

rapidcheck = dependency('rapidcheck')
deps_private += rapidcheck

gtest = dependency('gtest', main : true)
deps_private += gtest

libgit2 = dependency('libgit2')
deps_private += libgit2

subdir('bsd-meson-build-support/common')

sources = files(
  'access-tokens.cc',
  'git-utils.cc',
  'bsd_api_fetchers.cc',
  'public-key.cc',
)

include_dirs = [include_directories('.')]


this_exe = executable(
  meson.project_name(),
  sources,
  dependencies : deps_private_subproject + deps_private + deps_other,
  include_directories : include_dirs,
  # TODO: -lrapidcheck, see ../libutil-support/build.meson
  link_args: linker_export_flags + ['-lrapidcheck'],
  # get main from gtest
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
