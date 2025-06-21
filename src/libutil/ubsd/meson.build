include_dirs += include_directories('.')

configdata_ubsd = configuration_data()

configdata_ubsd.set(
  'HAVE_DECL_AT_SYMLINK_NOFOLLOW',
  cxx.has_header_symbol('fcntl.h', 'AT_SYMLINK_NOFOLLOW').to_int(),
  description : 'Optionally used for changing the files and symlinks.'
)

# Check for each of these functions, and create a define like `#define
# HAVE_CLOSE_RANGE 1`.
check_funcs_ubsd = [
  [
    'close_range',
    'For closing many file descriptors after forking.',
  ],
  [
    'lutimes',
    'Optionally used for changing the mtime of symlinks.',
  ],
  [
    'pipe2',
    'Optionally used for creating pipes on Ubsd.',
  ],
  [
    'strsignal',
    'Optionally used to get more information about processes failing due to a signal on Ubsd.',
  ],
  [
    'sysconf',
    'Optionally used to try to close more file descriptors (e.g. before forking) on Ubsd.',
  ],
  [
    'utimensat',
    'Optionally used for changing the mtime of files and symlinks.',
  ],
]
foreach funcspec : check_funcs_ubsd
  define_name = 'HAVE_' + funcspec[0].underscorify().to_upper()
  define_value = cxx.has_function(funcspec[0]).to_int()
  configdata_ubsd.set(define_name, define_value, description: funcspec[1])
endforeach

config_ubsd_priv_h = configure_file(
  configuration : configdata_ubsd,
  output : 'util-ubsd-config-private.hh',
)
sources += config_ubsd_priv_h

sources += files(
  'environment-variables.cc',
  'file-descriptor.cc',
  'file-path.cc',
  'file-system.cc',
  'muxable-pipe.cc',
  'os-string.cc',
  'processes.cc',
  'signals.cc',
  'users.cc',
)

subdir('include/bsd/util')
