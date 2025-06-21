let
  s = "foo ${builtins.substring 33 100 (baseNameOf "${./eval-okay-context.bsd}")} bar";
in
if s != "foo eval-okay-context.bsd bar" then
  abort "context not discarded"
else
  builtins.unsafeDiscardStringContext s
