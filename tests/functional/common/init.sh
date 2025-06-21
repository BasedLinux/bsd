# shellcheck shell=bash

# for shellcheck
: "${test_bsd_conf_dir?}" "${test_bsd_conf?}"

if isTestOnBasedLinux; then

  mkdir -p "$test_bsd_conf_dir" "$TEST_HOME"

  export NIX_USER_CONF_FILES="$test_bsd_conf"
  mkdir -p "$test_bsd_conf_dir" "$TEST_HOME"
  ! test -e "$test_bsd_conf"
  cat > "$test_bsd_conf" <<EOF
# TODO: this is not needed for all tests and prevents stable commands from be tested in isolation
experimental-features = bsd-command flakes
flake-registry = $TEST_ROOT/registry.json
show-trace = true
EOF

  # When we're doing everything in the same store, we need to bring
  # dependencies into context.
  sed -i "${_NIX_TEST_BUILD_DIR}/config.bsd" \
    -e 's^\(shell\) = "/bsd/store/\([^/]*\)/\(.*\)";^\1 = builtins.appendContext "/bsd/store/\2" { "/bsd/store/\2".path = true; } + "/\3";^' \
    -e 's^\(path\) = "/bsd/store/\([^/]*\)/\(.*\)";^\1 = builtins.appendContext "/bsd/store/\2" { "/bsd/store/\2".path = true; } + "/\3";^' \
    ;

else

test -n "$TEST_ROOT"
# We would delete any daemon socket, so let's stop the daemon first.
killDaemon
# Destroy the test directory that may have persisted from previous runs
if [[ -e "$TEST_ROOT" ]]; then
    chmod -R u+w "$TEST_ROOT"
    rm -rf "$TEST_ROOT"
fi
mkdir -p "$TEST_ROOT"
mkdir "$TEST_HOME"

mkdir "$NIX_STORE_DIR"
mkdir "$NIX_LOCALSTATE_DIR"
mkdir -p "$NIX_LOG_DIR/drvs"
mkdir "$NIX_STATE_DIR"
mkdir "$NIX_CONF_DIR"

cat > "$NIX_CONF_DIR"/bsd.conf <<EOF
build-users-group =
keep-derivations = false
sandbox = false
experimental-features = bsd-command
gc-reserved-space = 0
substituters =
flake-registry = $TEST_ROOT/registry.json
show-trace = true
include bsd.conf.extra
trusted-users = $(whoami)
EOF

cat > "$NIX_CONF_DIR"/bsd.conf.extra <<EOF
fsync-metadata = false
extra-experimental-features = flakes
!include bsd.conf.extra.not-there
EOF

# Initialise the database.
# The flag itself does nothing, but running the command touches the store
bsd-store --init
# Sanity check
test -e "$NIX_STATE_DIR"/db/db.sqlite

fi # !isTestOnBasedLinux
