#!/usr/bin/env bash

source common.sh

needLocalStore "--repair needs a local store"

TODO_BasedLinux

clearStore

path=$(bsd-build dependencies.nix -o $TEST_ROOT/result)
path2=$(bsd-store -qR $path | grep input-2)

bsd-store --verify --check-contents -v

hash=$(bsd-hash $path2)

# Corrupt a path and check whether bsd-build --repair can fix it.
chmod u+w $path2
touch $path2/bad

(! bsd-store --verify --check-contents -v)

# The path can be repaired by rebuilding the derivation.
bsd-store --verify --check-contents --repair

(! [ -e $path2/bad ])
(! [ -w $path2 ])

bsd-store --verify-path $path2

# Re-corrupt and delete the deriver. Now --verify --repair should
# not work.
chmod u+w $path2
touch $path2/bad

bsd-store --delete $(bsd-store -q --referrers-closure $(bsd-store -qd $path2))

(! bsd-store --verify --check-contents --repair)

bsd-build dependencies.nix -o $TEST_ROOT/result --repair

if [ "$(bsd-hash $path2)" != "$hash" -o -e $path2/bad ]; then
    echo "path not repaired properly" >&2
    exit 1
fi

# Corrupt a path that has a substitute and check whether bsd-store
# --verify can fix it.
clearCache

bsd copy --to file://$cacheDir $path

chmod u+w $path2
rm -rf $path2

bsd-store --verify --check-contents --repair --substituters "file://$cacheDir" --no-require-sigs

if [ "$(bsd-hash $path2)" != "$hash" -o -e $path2/bad ]; then
    echo "path not repaired properly" >&2
    exit 1
fi

# Check --verify-path and --repair-path.
bsd-store --verify-path $path2

chmod u+w $path2
rm -rf $path2

if bsd-store --verify-path $path2; then
    echo "bsd-store --verify-path succeeded unexpectedly" >&2
    exit 1
fi

bsd-store --repair-path $path2 --substituters "file://$cacheDir" --no-require-sigs

if [ "$(bsd-hash $path2)" != "$hash" -o -e $path2/bad ]; then
    echo "path not repaired properly" >&2
    exit 1
fi

# Check that --repair-path also checks content of optimised symlinks (1/2)
bsd-store --verify-path $path2

if (! bsd-store --optimize); then
    echo "bsd-store --optimize failed to optimize the store" >&2
    exit 1
fi
chmod u+w $path2/bar
echo 'rabrab' > $path2/bar # different length

if bsd-store --verify-path $path2; then
    echo "bsd-store --verify-path did not detect .links file corruption" >&2
    exit 1
fi

bsd-store --repair-path $path2 --option auto-optimise-store true

if [ "$(bsd-hash $path2)" != "$hash" -o "BAR" != "$(< $path2/bar)" ]; then
    echo "path not repaired properly" >&2
    exit 1
fi

# Check that --repair-path also checks content of optimised symlinks (2/2)
bsd-store --verify-path $path2

if (! bsd-store --optimize); then
    echo "bsd-store --optimize failed to optimize the store" >&2
    exit 1
fi
chmod u+w $path2
chmod u+w $path2/bar
sed -e 's/./X/g' < $path2/bar > $path2/tmp # same length, different content.
cp $path2/tmp $path2/bar
rm $path2/tmp

if bsd-store --verify-path $path2; then
    echo "bsd-store --verify-path did not detect .links file corruption" >&2
    exit 1
fi

bsd-store --repair-path $path2 --substituters "file://$cacheDir" --no-require-sigs --option auto-optimise-store true

if [ "$(bsd-hash $path2)" != "$hash" -o "BAR" != "$(< $path2/bar)" ]; then
    echo "path not repaired properly" >&2
    exit 1
fi
