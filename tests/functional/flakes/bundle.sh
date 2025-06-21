#!/usr/bin/env bash

source common.sh

cp ../simple.bsd ../simple.builder.sh "${config_bsd}" "$TEST_HOME"

cd "$TEST_HOME"

cat <<EOF > flake.bsd
{
    outputs = {self}: {
      bundlers.$system = rec {
        simple = drv:
          if drv?type && drv.type == "derivation"
          then drv
          else self.packages.$system.default;
        default = simple;
      };
      packages.$system.default = import ./simple.bsd;
      apps.$system.default = {
        type = "app";
        program = "\${import ./simple.bsd}/hello";
      };
    };
}
EOF

bsd build .#
bsd bundle --bundler .# .#
bsd bundle --bundler .#bundlers."$system".default .#packages."$system".default
bsd bundle --bundler .#bundlers."$system".simple  .#packages."$system".default

bsd bundle --bundler .#bundlers."$system".default .#apps."$system".default
bsd bundle --bundler .#bundlers."$system".simple  .#apps."$system".default
