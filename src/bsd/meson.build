project('bsd', 'cpp',
  version : files('.version'),
  default_options : [
    'cpp_std=c++2a',
    # TODO(Qyriad): increase the warning level
    'warning_level=1',
    'errorlogs=true', # Please print logs for tests that fail
    'localstatedir=/bsd/var',
  ],
  meson_version : '>= 1.4',
  license : 'LGPL-2.1-or-later',
)

cxx = meson.get_compiler('cpp')

subdir('bsd-meson-build-support/deps-lists')

bsd_store = dependency('bsd-store')

deps_private_maybe_subproject = [
  dependency('bsd-util'),
  bsd_store,
  dependency('bsd-expr'),
  dependency('bsd-flake'),
  dependency('bsd-fetchers'),
  dependency('bsd-main'),
  dependency('bsd-cmd'),
]
deps_public_maybe_subproject = [
]
subdir('bsd-meson-build-support/subprojects')

subdir('bsd-meson-build-support/export-all-symbols')
subdir('bsd-meson-build-support/windows-version')

configdata = configuration_data()

# The CLI has a more detailed version string than the libraries; see `bsdVersion`
configdata.set_quoted('NIX_CLI_VERSION', meson.project_version())

fs = import('fs')
prefix = get_option('prefix')

bindir = get_option('bindir')
bindir = fs.is_absolute(bindir) ? bindir : prefix / bindir
configdata.set_quoted('NIX_BIN_DIR', bindir)

mandir = get_option('mandir')
mandir = fs.is_absolute(mandir) ? mandir : prefix / mandir
configdata.set_quoted('NIX_MAN_DIR', mandir)

config_priv_h = configure_file(
  configuration : configdata,
  output : 'cli-config-private.hh',
)

subdir('bsd-meson-build-support/common')
subdir('bsd-meson-build-support/generate-header')

bsd_sources = [config_priv_h] + files(
  'add-to-store.cc',
  'app.cc',
  'self-exe.cc',
  'build.cc',
  'bundle.cc',
  'cat.cc',
  'config-check.cc',
  'config.cc',
  'copy.cc',
  'crash-handler.cc',
  'derivation-add.cc',
  'derivation-show.cc',
  'derivation.cc',
  'develop.cc',
  'diff-closures.cc',
  'dump-path.cc',
  'edit.cc',
  'env.cc',
  'eval.cc',
  'flake.cc',
  'formatter.cc',
  'hash.cc',
  'log.cc',
  'ls.cc',
  'main.cc',
  'make-content-addressed.cc',
  'man-pages.cc',
  'nar.cc',
  'optimise-store.cc',
  'path-from-hash-part.cc',
  'path-info.cc',
  'prefetch.cc',
  'profile.cc',
  'realisation.cc',
  'registry.cc',
  'repl.cc',
  'run.cc',
  'search.cc',
  'sigs.cc',
  'store-copy-log.cc',
  'store-delete.cc',
  'store-gc.cc',
  'store-info.cc',
  'store-repair.cc',
  'store.cc',
  'upgrade-bsd.cc',
  'verify.cc',
  'why-depends.cc',
)

if host_machine.system() != 'windows'
  bsd_sources += files(
    'ubsd/daemon.cc',
  )
endif

bsd_sources += [
  gen_header.process('doc/manual/generate-manpage.bsd'),
  gen_header.process('doc/manual/generate-settings.bsd'),
  gen_header.process('doc/manual/generate-store-info.bsd'),
  gen_header.process('doc/manual/utils.bsd'),
  gen_header.process('get-env.sh'),
  gen_header.process('profiles.md'),
  gen_header.process('help-stores.md'),
]

# The rest of the subdirectories aren't separate components,
# just source files in another directory, so we process them here.

build_remote_sources = files(
  'build-remote/build-remote.cc',
)
bsd_build_sources = files(
  'bsd-build/bsd-build.cc',
)
bsd_channel_sources = files(
  'bsd-channel/bsd-channel.cc',
)
unpack_channel_gen = gen_header.process('bsd-channel/unpack-channel.bsd')
bsd_collect_garbage_sources = files(
  'bsd-collect-garbage/bsd-collect-garbage.cc',
)
bsd_copy_closure_sources = files(
  'bsd-copy-closure/bsd-copy-closure.cc',
)
bsd_env_buildenv_gen = gen_header.process('bsd-env/buildenv.bsd')
bsd_env_sources = files(
  'bsd-env/bsd-env.cc',
  'bsd-env/user-env.cc',
)
bsd_instantiate_sources = files(
  'bsd-instantiate/bsd-instantiate.cc',
)
bsd_store_sources = files(
  'bsd-store/dotgraph.cc',
  'bsd-store/graphml.cc',
  'bsd-store/bsd-store.cc',
)

# Hurray for Meson list flattening!
sources = [
  bsd_sources,
  bsd_build_sources,
  unpack_channel_gen,
  bsd_collect_garbage_sources,
  bsd_copy_closure_sources,
  bsd_env_buildenv_gen,
  bsd_env_sources,
  bsd_instantiate_sources,
  bsd_store_sources,
]

if host_machine.system() != 'windows'
  sources += [
    build_remote_sources,
    bsd_channel_sources,
  ]
endif

include_dirs = [include_directories('.')]

this_exe = executable(
  meson.project_name(),
  sources,
  dependencies : deps_private_subproject + deps_private + deps_other,
  include_directories : include_dirs,
  link_args: linker_export_flags,
  install : true,
)

meson.override_find_program('bsd', this_exe)

bsd_symlinks = [
  'bsd-build',
  'bsd-channel',
  'bsd-collect-garbage',
  'bsd-copy-closure',
  'bsd-daemon',
  'bsd-env',
  'bsd-hash',
  'bsd-instantiate',
  'bsd-prefetch-url',
  'bsd-shell',
  'bsd-store',
]

executable_suffix = ''
if host_machine.system() == 'windows'
  executable_suffix = '.exe'
endif

foreach linkname : bsd_symlinks
  install_symlink(
    linkname + executable_suffix,
    # TODO(Qyriad): should these continue to be relative symlinks?
    pointing_to : fs.name(this_exe),
    install_dir : get_option('bindir'),
    # The 'runtime' tag is what executables default to, which we want to emulate here.
    install_tag : 'runtime'
  )
  custom_target(
    command: ['ln', '-sf', fs.name(this_exe), '@OUTPUT@'],
    output: linkname + executable_suffix,
    # native doesn't allow dangling symlinks, but the target executable often doesn't exist at this time
    env : { 'MSYS' : 'winsymlinks:lnk' },
    # TODO(Ericson2314): Don't do this once we have the `meson.override_find_program` working)
    build_by_default: true
  )
  # TODO(Ericson3214): Doesn't yet work
  #meson.override_find_program(linkname, t)
endforeach

install_symlink(
  'build-remote',
  pointing_to : '..' / '..'/ get_option('bindir') / fs.name(this_exe),
  install_dir : get_option('libexecdir') / fs.name(this_exe),
  # The 'runtime' tag is what executables default to, which we want to emulate here.
  install_tag : 'runtime'
)

custom_target(
  command: ['ln', '-sf', fs.name(this_exe), '@OUTPUT@'],
  output: 'build-remote' + executable_suffix,
  # native doesn't allow dangling symlinks, but the target executable often doesn't exist at this time
  env : { 'MSYS' : 'winsymlinks:lnk' },
  # TODO(Ericson2314): Don't do this once we have the `meson.override_find_program` working)
  build_by_default: true
)
# TODO(Ericson3214): Doesn't yet work
#meson.override_find_program(linkname, t)

localstatedir = bsd_store.get_variable(
  'localstatedir',
  default_value : get_option('localstatedir'),
)
assert(localstatedir == get_option('localstatedir'))
store_dir = bsd_store.get_variable('storedir')
subdir('scripts')
subdir('misc')
