# Bsd-Perl Scripts
#============================================================================



# Sources
#-------------------------------------------------

bsd_perl_store_xs = files('Store.xs')

bsd_perl_scripts = files(
  'CopyClosure.pm',
  'Manifest.pm',
  'SSH.pm',
  'Store.pm',
  'Utils.pm',
)

foreach f : bsd_perl_scripts
  fs.copyfile(f)
endforeach


# Targets
#---------------------------------------------------

bsd_perl_scripts += configure_file(
  output : 'Config.pm',
  input : 'Config.pm.in',
  configuration : bsd_perl_conf,
)

bsd_perl_store_cc = custom_target(
  'Store.cc',
  output : 'Store.cc',
  input : bsd_perl_store_xs,
  command : [xsubpp, '@INPUT@', '-output', '@OUTPUT@'],
)

# Build Bsd::Store Library
#-------------------------------------------------
bsd_perl_store_lib = library(
  'Store',
  sources : bsd_perl_store_cc,
  name_prefix  : '',
  prelink : true, # For C++ static initializers
  install : true,
  install_mode : 'rwxr-xr-x',
  install_dir : join_paths(bsd_perl_install_dir, 'auto', 'Bsd', 'Store'),
  dependencies : bsd_perl_store_dep_list,
)


# Install Scripts
#---------------------------------------------------
install_data(
  bsd_perl_scripts,
  install_mode : 'rw-r--r--',
  install_dir : join_paths(bsd_perl_install_dir,'Bsd'),
)
