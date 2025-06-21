configure_file(
  input : 'bsd-profile.sh.in',
  output : 'bsd-profile.sh',
  configuration : {
    'localstatedir': localstatedir,
  }
)

foreach rc : [ '.sh', '.fish', '-daemon.sh', '-daemon.fish' ]
  configure_file(
    input : 'bsd-profile' + rc  + '.in',
    output : 'bsd' + rc,
    install : true,
    install_dir : get_option('profile-dir'),
    install_mode : 'rw-r--r--',
    configuration : {
      'localstatedir': localstatedir,
    },
  )
endforeach
