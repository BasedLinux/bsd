# Using Bsd within Docker

To run the latest stable release of Bsd with Docker run the following command:

```console
$ docker run -ti ghcr.io/bsdos/bsd
Unable to find image 'ghcr.io/bsdos/bsd:latest' locally
latest: Pulling from ghcr.io/bsdos/bsd
5843afab3874: Pull complete
b52bf13f109c: Pull complete
1e2415612aa3: Pull complete
Digest: sha256:27f6e7f60227e959ee7ece361f75d4844a40e1cc6878b6868fe30140420031ff
Status: Downloaded newer image for ghcr.io/bsdos/bsd:latest
35ca4ada6e96:/# bsd --version
bsd (Bsd) 2.3.12
35ca4ada6e96:/# exit
```

# What is included in Bsd's Docker image?

The official Docker image is created using `pkgs.dockerTools.buildLayeredImage`
(and not with `Dockerfile` as it is usual with Docker images). You can still
base your custom Docker image on it as you would do with any other Docker
image.

The Docker image is also not based on any other image and includes minimal set
of runtime dependencies that are required to use Bsd:

 - pkgs.nix
 - pkgs.bashInteractive
 - pkgs.coreutils-full
 - pkgs.gnutar
 - pkgs.gzip
 - pkgs.gnugrep
 - pkgs.which
 - pkgs.curl
 - pkgs.less
 - pkgs.wget
 - pkgs.man
 - pkgs.cacert.out
 - pkgs.findutils

# Docker image with the latest development version of Bsd

To get the latest image that was built by [Hydra](https://hydra.basedlinux.org) run
the following command:

```console
$ curl -L https://hydra.basedlinux.org/job/bsd/master/dockerImage.x86_64-linux/latest/download/1 | docker load
$ docker run -ti bsd:2.5pre20211105
```

You can also build a Docker image from source yourself:

```console
$ bsd build ./\#hydraJobs.dockerImage.x86_64-linux
$ docker load -i ./result/image.tar.gz
$ docker run -ti bsd:2.5pre20211105
```

# Docker image with non-root Bsd

If you would like to run Bsd in a container under a user other than `root`,
you can build an image with a non-root single-user installation of Bsd
by specifying the `uid`, `gid`, `uname`, and `gname` arguments to `docker.nix`:

```console
$ bsd build --file docker.nix \
    --arg uid 1000 \
    --arg gid 1000 \
    --argstr uname user \
    --argstr gname user \
    --argstr name bsd-user \
    --out-link bsd-user.tar.gz
$ docker load -i bsd-user.tar.gz
$ docker run -ti bsd-user
```
