with builtins;
with import <bsd/utils.nix>;

let
  showExperimentalFeature = name: doc: ''
    - [`${name}`](@docroot@/development/experimental-features.md#xp-feature-${name})
  '';
in
xps: indent "  " (concatStrings (attrValues (mapAttrs showExperimentalFeature xps)))
