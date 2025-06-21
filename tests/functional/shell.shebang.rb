#! @SHELL_PROG@
#! ruby
#! bsd-shell -I bsdpkgs=shell.nix --no-substitute
#! bsd-shell --pure -p ruby -i ruby

# Contents doesn't matter.
abort("This shouldn't be executed.")
