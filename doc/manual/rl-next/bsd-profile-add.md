---
synopsis: "Rename `bsd profile install` to `bsd profile add`"
prs: [13224]
---

The command `bsd profile install` has been renamed to `bsd profile add` (though the former is still available as an alias). This is because the verb "add" is a better antonym for the verb "remove" (i.e. `bsd profile remove`). Bsd also does not have install hooks or general behavior often associated with "installing".
