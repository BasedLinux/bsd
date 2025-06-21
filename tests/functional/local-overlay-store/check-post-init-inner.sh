#!/usr/bin/env bash

set -eu -o pipefail

set -x

source common.sh

# Avoid store dir being inside sandbox build-dir
unset NIX_STORE_DIR
unset NIX_STATE_DIR

setupStoreDirs

initLowerStore

mountOverlayfs

### Check status

# Checking for path in lower layer
stat $(toRealPath "$storeA/bsd/store" "$pathInLowerStore")

# Checking for path in upper layer (should fail)
expect 1 stat $(toRealPath "$storeBTop" "$pathInLowerStore")

# Checking for path in overlay store matching lower layer
diff $(toRealPath "$storeA/bsd/store" "$pathInLowerStore") $(toRealPath "$storeBRoot/bsd/store" "$pathInLowerStore")

# Checking requisites query agreement
[[ \
  $(bsd-store --store $storeA --query --requisites $drvPath) \
  == \
  $(bsd-store --store $storeB --query --requisites $drvPath) \
  ]]

# Checking referrers query agreement
busyboxStore=$(bsd store --store $storeA add-path $busybox)
[[ \
  $(bsd-store --store $storeA --query --referrers $busyboxStore) \
  == \
  $(bsd-store --store $storeB --query --referrers $busyboxStore) \
  ]]

# Checking derivers query agreement
[[ \
  $(bsd-store --store $storeA --query --deriver $pathInLowerStore) \
  == \
  $(bsd-store --store $storeB --query --deriver $pathInLowerStore) \
  ]]

# Checking outputs query agreement
[[ \
  $(bsd-store --store $storeA --query --outputs $drvPath) \
  == \
  $(bsd-store --store $storeB --query --outputs $drvPath) \
  ]]

# Verifying path in lower layer
bsd-store --verify-path --store "$storeA" "$pathInLowerStore"

# Verifying path in merged-store
bsd-store --verify-path --store "$storeB" "$pathInLowerStore"

hashPart=$(echo $pathInLowerStore | sed "s^${NIX_STORE_DIR:-/bsd/store}/^^" | sed 's/-.*//')

# Lower store can find from hash part
[[ $(bsd store --store $storeA path-from-hash-part $hashPart) == $pathInLowerStore ]]

# merged store can find from hash part
[[ $(bsd store --store $storeB path-from-hash-part $hashPart) == $pathInLowerStore ]]
