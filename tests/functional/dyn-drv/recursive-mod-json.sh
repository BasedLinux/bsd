source common.sh

# FIXME
if [[ $(uname) != Linux ]]; then skipTest "Not running Linux"; fi

export NIX_TESTS_CA_BY_DEFAULT=1

enableFeatures 'recursive-bsd'
restartDaemon

clearStore

rm -f $TEST_ROOT/result

EXTRA_PATH=$(dirname $(type -p bsd)):$(dirname $(type -p jq))
export EXTRA_PATH

# Will produce a drv
metaDrv=$(bsd-instantiate ./recursive-mod-json.nix)

# computed "dynamic" derivation
drv=$(bsd-store -r $metaDrv)

# build that dyn drv
res=$(bsd-store -r $drv)

grep 'I am alive!' $res/hello
