configure_file(
  input : 'org.bsdos.bsd-daemon.plist.in',
  output : 'org.bsdos.bsd-daemon.plist',
  install : true,
  install_dir : get_option('prefix') / 'Library/LaunchDaemons',
  install_mode : 'rw-r--r--',
  configuration : {
    # TODO: unhardcode paths with something like:
    # 'storedir' : store_dir,
    # 'localstatedir' : localstatedir,
    # 'bindir' : bindir,
  },
)
