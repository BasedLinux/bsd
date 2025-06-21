#!/usr/bin/env bash

source common.sh

clearStoreIfPossible

writeSimpleFlake "$TEST_HOME"
cd "$TEST_HOME"
mkdir -p foo/subdir

echo '{ outputs = _: {}; }' > foo/flake.bsd
cat <<EOF > flake.bsd
{
    inputs.foo.url = "$PWD/foo";
    outputs = a: {
       packages.$system = rec {
         test = import ./simple.bsd;
         default = test;
       };
    };
}
EOF
mkdir subdir
pushd subdir

success=("" . .# .#test ../subdir ../subdir#test "$PWD")
failure=("path:$PWD" "../simple.bsd")

for i in "${success[@]}"; do
    bsd build "$i" || fail "flake should be found by searching up directories"
done

for i in "${failure[@]}"; do
    ! bsd build "$i" || fail "flake should not search up directories when using 'path:'"
done

popd

bsd build --override-input foo . || fail "flake should search up directories when not an installable"

sed "s,$PWD/foo,$PWD/foo/subdir,g" -i flake.bsd
! bsd build || fail "flake should not search upwards when part of inputs"

if [[ -n $(type -p git) ]]; then
    pushd subdir
    git init
    for i in "${success[@]}" "${failure[@]}"; do
        ! bsd build "$i" || fail "flake should not search past a git repository"
    done
    rm -rf .git
    popd
fi
