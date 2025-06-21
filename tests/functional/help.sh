#!/usr/bin/env bash

source common.sh

function subcommands() {
  jq -r '
def recurse($prefix):
    to_entries[] |
    ($prefix + [.key]) as $newPrefix |
    (if .value | has("commands") then
      ($newPrefix, (.value.commands | recurse($newPrefix)))
    else
      $newPrefix
    end);
.args.commands | recurse([]) | join(" ")
'
}

bsd __dump-cli | subcommands | while IFS= read -r cmd; do
    # shellcheck disable=SC2086 # word splitting of cmd is intended
    bsd $cmd --help
done

[[ $(type -p man) ]] || skipTest "'man' not installed"

# FIXME: we don't know whether we built the manpages, so we can't
# reliably test them here.
skipTest "we don't know whether we built the manpages, so we can't reliably test them here."

# test help output

bsd-build --help
bsd-shell --help

bsd-env --help
bsd-env --install --help
bsd-env --upgrade --help
bsd-env --uninstall --help
bsd-env --set --help
bsd-env --set-flag --help
bsd-env --query --help
bsd-env --switch-profile --help
bsd-env --list-generations --help
bsd-env --delete-generations --help
bsd-env --switch-generation --help
bsd-env --rollback --help

bsd-store --help
bsd-store --realise --help
bsd-store --serve --help
bsd-store --gc --help
bsd-store --delete --help
bsd-store --query --help
bsd-store --add --help
bsd-store --add-fixed --help
bsd-store --verify --help
bsd-store --verify-path --help
bsd-store --repair-path --help
bsd-store --dump --help
bsd-store --restore --help
bsd-store --export --help
bsd-store --import --help
bsd-store --optimise --help
bsd-store --read-log --help
bsd-store --dump-db --help
bsd-store --load-db --help
bsd-store --print-env --help
bsd-store --generate-binary-cache-key --help

bsd-channel --help
bsd-collect-garbage --help
bsd-copy-closure --help
bsd-daemon --help
bsd-hash --help
bsd-instantiate --help
bsd-prefetch-url --help
