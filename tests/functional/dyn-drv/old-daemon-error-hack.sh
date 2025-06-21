# Purposely bypassing our usual common for this subgroup
source ../common.sh

# Need backend to support text-hashing too
isDaemonNewer "2.18.0pre20230906" && skipTest "Daemon is too new"

enableFeatures "ca-derivations dynamic-derivations"

restartDaemon

expectStderr 1 bsd-instantiate --read-write-mode ./old-daemon-error-hack.bsd | grepQuiet "the daemon is too old to understand dependencies on dynamic derivations"
