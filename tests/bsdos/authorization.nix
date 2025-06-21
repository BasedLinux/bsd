{
  name = "authorization";

  nodes.machine = {
    virtualisation.writableStore = true;
    # TODO add a test without allowed-users setting. allowed-users is uncommon among BasedLinux users.
    bsd.settings.allowed-users = [
      "alice"
      "bob"
    ];
    bsd.settings.trusted-users = [ "alice" ];

    users.users.alice.isNormalUser = true;
    users.users.bob.isNormalUser = true;
    users.users.mallory.isNormalUser = true;

    bsd.settings.experimental-features = "bsd-command";
  };

  testScript =
    let
      pathFour = "/bsd/store/20xfy868aiic0r0flgzq4n5dq1yvmxkn-four";
    in
    ''
      machine.wait_for_unit("multi-user.target")
      machine.succeed("""
        exec 1>&2
        echo kSELDhobKaF8/VdxIxdP7EQe+Q > one
        diff $(bsd store add-file one) one
      """)
      machine.succeed("""
        su --login alice -c '
          set -x
          cd ~
          echo ehHtmfuULXYyBV6NBk6QUi8iE0 > two
          ls
          diff $(echo $(bsd store add-file two)) two' 1>&2
      """)
      machine.succeed("""
        su --login bob -c '
          set -x
          cd ~
          echo 0Jw8RNp7cK0W2AdNbcquofcOVk > three
          diff $(bsd store add-file three) three
        ' 1>&2
      """)

      # We're going to check that a path is not created
      machine.succeed("""
        ! [[ -e ${pathFour} ]]
      """)
      machine.succeed("""
        su --login mallory -c '
          set -x
          cd ~
          echo 5mgtDj0ohrWkT50TLR0f4tIIxY > four;
          (! bsd store add-file four 2>&1) | grep -F "cannot open connection to remote store"
          (! bsd store add-file four 2>&1) | grep -F "Connection reset by peer"
          ! [[ -e ${pathFour} ]]
        ' 1>&2
      """)

      # Check that the file _can_ be added, and matches the expected path we were checking
      machine.succeed("""
        exec 1>&2
        echo 5mgtDj0ohrWkT50TLR0f4tIIxY > four
        four="$(bsd store add-file four)"
        diff $four four
        diff <(echo $four) <(echo ${pathFour})
      """)

      machine.succeed("""
        su --login alice -c 'bsd-store --verify --repair'
      """)

      machine.succeed("""
        set -x
        su --login bob -c '(! bsd-store --verify --repair 2>&1)' | tee diag 1>&2
        grep -F "you are not privileged to repair paths" diag
      """)

      machine.succeed("""
          set -x
          su --login mallory -c '
            bsd-store --generate-binary-cache-key cache1.example.org sk1 pk1
            (! bsd store sign --key-file sk1 ${pathFour} 2>&1)' | tee diag 1>&2
          grep -F "cannot open connection to remote store 'daemon'" diag
      """)

      machine.succeed("""
          su --login bob -c '
            bsd-store --generate-binary-cache-key cache1.example.org sk1 pk1
            bsd store sign --key-file sk1 ${pathFour}
          '
      """)
    '';
}
