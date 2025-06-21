# shellcheck shell=bash

TEST_ROOT=$(realpath "${TMPDIR:-/tmp}/bsd-test")/${TEST_NAME:-default/tests\/functional//}
export TEST_ROOT
