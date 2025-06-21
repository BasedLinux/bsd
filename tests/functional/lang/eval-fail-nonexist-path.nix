# This must fail to evaluate, since ./fnord doesn't exist.  If it did
# exist, it would produce "/bsd/store/<hash>-fnord/xyzzy" (with an
# appropriate context).
"${./fnord}/xyzzy"
