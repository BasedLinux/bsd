{
  name = "functional-tests-on-bsdos_root";

  imports = [ ./common.bsd ];

  testScript = ''
    machine.wait_for_unit("multi-user.target")
    machine.succeed("""
      run-test-suite >&2
    """)
  '';
}
