{
  inBsdShell ? false,
  ...
}@args:
import ./shell.bsd (args // { contentAddressed = true; })
