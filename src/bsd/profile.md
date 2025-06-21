R""(

# Description

`bsd profile` allows you to create and manage *Bsd profiles*. A Bsd
profile is a set of packages that can be installed and upgraded
independently from each other. Bsd profiles are versioned, allowing
them to be rolled back easily.

# Files

)""

#include "profiles.md.gen.hh"

R""(

### Profile compatibility

> **Warning**
>
> Once you have used [`bsd profile`] you can no longer use [`bsd-env`] without first deleting `$XDG_STATE_HOME/bsd/profiles/profile`

[`bsd-env`]: @docroot@/command-ref/bsd-env.md
[`bsd profile`]: @docroot@/command-ref/new-cli/bsd3-profile.md

Once you installed a package with [`bsd profile`], you get the following error message when using [`bsd-env`]:

```console
$ bsd-env -f '<bsdpkgs>' -iA 'hello'
error: bsd-env
profile '/home/alice/.local/state/bsd/profiles/profile' is incompatible with 'bsd-env'; please use 'bsd profile' instead
```

To migrate back to `bsd-env` you can delete your current profile:

> **Warning**
>
> This will delete packages that have been installed before, so you may want to back up this information before running the command.

```console
 $ rm -rf "${XDG_STATE_HOME-$HOME/.local/state}/bsd/profiles/profile"
```

)""
