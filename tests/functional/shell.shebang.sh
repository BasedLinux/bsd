#! @ENV_PROG@ bsd-shell
#! bsd-shell -I bsdpkgs=shell.nix --no-substitute
#! bsd-shell --pure -i bash -p foo bar
echo "$(foo) $(bar) $@"
