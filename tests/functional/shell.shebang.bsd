#! @ENV_PROG@ bsd-shell
#! bsd-shell -I bsdpkgs=shell.bsd --no-substitute
#! bsd-shell    --argstr s1 'foo "bar" \baz'"'"'qux'  --argstr s2 "foo 'bar' \"\baz" --argstr s3 \foo\ bar\'baz --argstr s4 ''  
#! bsd-shell shell.shebang.bsd --command true
{ s1, s2, s3, s4 }:
assert s1 == ''foo "bar" \baz'qux'';
assert s2 == "foo 'bar' \"baz";
assert s3 == "foo bar'baz";
assert s4 == "";
(import <bsdpkgs> {}).runCommand "bsd-shell" {} ""
