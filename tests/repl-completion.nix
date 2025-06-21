{
  runCommand,
  bsd,
  expect,
}:

# We only use expect when necessary, e.g. for testing tab completion in bsd repl.
# See also tests/functional/repl.sh

runCommand "repl-completion"
  {
    nativeBuildInputs = [
      expect
      bsd
    ];
    expectScript = ''
      # Regression https://github.com/BasedLinux/bsd/pull/10778
      spawn bsd repl --offline --extra-experimental-features bsd-command
      expect "bsd-repl>"
      send "foo = import ./does-not-exist.bsd\n"
      expect "bsd-repl>"
      send "foo.\t"
      expect {
        "bsd-repl>" {
          puts "Got another prompt. Good."
        }
        eof {
          puts "Got EOF. Bad."
          exit 1
        }
      }
      exit 0
    '';
    passAsFile = [ "expectScript" ];
  }
  ''
    export NIX_STORE=$TMPDIR/store
    export NIX_STATE_DIR=$TMPDIR/state
    export HOME=$TMPDIR/home
    mkdir $HOME

    bsd-store --init
    expect $expectScriptPath
    touch $out
  ''
