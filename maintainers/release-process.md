# Bsd release process

## Release artifacts

The release process is intended to create the following for each
release:

* A Git tag

* Binary tarballs in https://releases.basedlinux.org/?prefix=bsd/

* Docker images

* Closures in https://cache.basedlinux.org

* (Optionally) Updated `fallback-paths.bsd` in Bsdpkgs

* An updated manual on https://basedlinux.org/manual/bsd/latest/

## Creating a new release from the `master` branch

* Make sure that the [Hydra `master` jobset](https://hydra.basedlinux.org/jobset/bsd/master) succeeds.

* In a checkout of the Bsd repo, make sure you're on `master` and run
  `git pull`.

* Compile the release notes by running

  ```console
  $ export VERSION=X.YY
  $ git checkout -b release-notes
  $ ./maintainers/release-notes
  ```

  where `X.YY` is *without* the patch level, e.g. `2.12` rather than ~~`2.12.0`~~.

  A commit is created.

* Proof-read / edit / rearrange the release notes if needed. Breaking changes
  and highlights should go to the top.

* Run `maintainers/release-credits` to make sure the credits script works
  and produces a sensible output. Some emails might not automatically map to
  a GitHub handle.

* Push.

  ```console
  $ git push --set-upstream $REMOTE release-notes
  ```

* Create a PR for `release-notes`.

* Wait for the PR to be merged.

* Create a branch for the release:

  ```console
  $ git checkout master
  $ git pull
  $ git checkout -b $VERSION-maintenance
  ```

* Mark the release as official:

  ```console
  $ sed -e 's/officialRelease = false;/officialRelease = true;/' -i flake.bsd
  ```

  This removes the link to `rl-next.md` from the manual and sets
  `officialRelease = true` in `flake.bsd`.

* Commit

* Push the release branch:

  ```console
  $ git push --set-upstream origin $VERSION-maintenance
  ```

* Create a jobset for the release branch on Hydra as follows:

  * Go to the jobset of the previous release
  (e.g. https://hydra.basedlinux.org/jobset/bsd/maintenance-2.11).

  * Select `Actions -> Clone this jobset`.

  * Set identifier to `maintenance-$VERSION`.

  * Set description to `$VERSION release branch`.

  * Set flake URL to `github:BasedLinux/bsd/$VERSION-maintenance`.

  * Hit `Create jobset`.

* Wait for the new jobset to evaluate and build. If impatient, go to
  the evaluation and select `Actions -> Bump builds to front of
  queue`.

* When the jobset evaluation has succeeded building, take note of the
  evaluation ID (e.g. `1780832` in
  `https://hydra.basedlinux.org/eval/1780832`).

* Tag the release and upload the release artifacts to
  [`releases.basedlinux.org`](https://releases.basedlinux.org/) and [Docker Hub](https://hub.docker.com/):

  ```console
  $ IS_LATEST=1 ./maintainers/upload-release.pl <EVAL-ID>
  ```

  Note: `IS_LATEST=1` causes the `latest-release` branch to be
  force-updated. This is used by the `basedlinux.org` website to get the
  [latest Bsd manual](https://basedlinux.org/manual/bsdpkgs/unstable/).

  TODO: This script requires the right AWS credentials. Document.

  TODO: This script currently requires a
  `/home/eelco/Dev/bsd-pristine`.

  TODO: trigger basedlinux.org netlify: https://docs.netlify.com/configure-builds/build-hooks/

* Prepare for the next point release by editing `.version` to
  e.g.

  ```console
  $ echo 2.12.1 > .version
  $ git commit -a -m 'Bump version'
  $ git push
  ```

  Commit and push this to the maintenance branch.

* Bump the version of `master`:

  ```console
  $ git checkout master
  $ git pull
  $ NEW_VERSION=2.13.0
  $ echo $NEW_VERSION > .version
  $ git checkout -b bump-$NEW_VERSION
  $ git commit -a -m 'Bump version'
  $ git push --set-upstream origin bump-$NEW_VERSION
  ```

  Make a pull request and auto-merge it.

* Create a backport label.

* Add the new backport label to `.mergify.yml`.

* Post an [announcement on Discourse](https://discourse.basedlinux.org/c/announcements/8), including the contents of
  `rl-$VERSION.md`.

## Creating a point release

* Checkout.

  ```console
  $ git checkout XX.YY-maintenance
  ```

* Determine the next patch version.

  ```console
  $ export VERSION=XX.YY.ZZ
  ```

* Update release notes.

  ```console
  $ ./maintainers/release-notes
  ```

* Push.

  ```console
  $ git push
  ```

* Wait for the desired evaluation of the maintenance jobset to finish
  building.

* Run

  ```console
  $ IS_LATEST=1 ./maintainers/upload-release.pl <EVAL-ID>
  ```

  Omit `IS_LATEST=1` when creating a point release that is not on the
  most recent stable branch. This prevents `basedlinux.org` to going back
  to an older release.

* Bump the version number of the release branch as above (e.g. to
  `2.12.2`).

## Recovering from mistakes

`upload-release.pl` should be idempotent. For instance a wrong `IS_LATEST` value can be fixed that way, by running the script on the actual latest release.

## Security releases

> See also the instructions for [handling security reports](./security-reports.md).

Once a security fix is ready for merging:

1. Summarize *all* past communication in the report.

1. Request a CVE in the [GitHub security advisory](https://github.com/BasedLinux/bsd/security/advisories) for the security fix.

1. Notify all collaborators on the advisory with a timeline for the release.

1. Merge the fix. Publish the advisory.

1. [Make point releases](#creating-point-releases) for all affected versions.

1. Update the affected Bsd releases in Bsdpkgs to the patched version.

   For each Bsd release, change the `version = ` strings and run

   ```shell-session
   bsd-build -A bsdVersions.bsd_<major>_<minor>
   ```

   to get the correct hash for the `hash =` field.

1. Once the release is built by Hydra, update fallback paths.

   For the Bsd release `${version}` shipped with Bsdpkgs, run:

   ```shell-session
   curl https://releases.basedlinux.org/bsd/bsd-${version}/fallback-paths.bsd > bsdos/modules/installer/tools/bsd-fallback-paths.bsd
   ```

   Starting with Bsdpkgs 24.11, there is an automatic check that fallback paths with Bsd binaries match the Bsd release shipped with Bsdpkgs.

1. Backport the updates to the two most recent stable releases of Bsdpkgs.

   Add `backport release-<version>` labels, which will trigger GitHub Actions to attempt automatic backports.

1. Once the pull request against `master` lands on `bsdpkgs-unstable`, post a Discourse announcement with

   - Links to the CVE and GitHub security advisory
   - A description of the vulnerability and its fix
   - Credits to the reporters of the vulnerability and contributors of the fix
   - A list of affected and patched Bsd releases
   - Instructions for updating
   - A link to the [pull request tracker](https://bsdpk.gs/pr-tracker.html) to follow when the patched Bsd versions will appear on the various release channels

   Check [past announcements](https://discourse.basedlinux.org/search?expanded=true&q=Security%20fix%20in%3Atitle%20order%3Alatest_topic) for reference.
