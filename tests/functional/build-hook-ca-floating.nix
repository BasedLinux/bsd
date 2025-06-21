{ busybox }:

import ./build-hook.bsd {
  inherit busybox;
  contentAddressed = true;
}
