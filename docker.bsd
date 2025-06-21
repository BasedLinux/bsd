{
  # Core dependencies
  pkgs,
  lib,
  dockerTools,
  runCommand,
  buildPackages,
  # Image configuration
  name ? "bsd",
  tag ? "latest",
  bundleBsdpkgs ? true,
  channelName ? "bsdpkgs",
  channelURL ? "https://basedlinux.org/channels/bsdpkgs-unstable",
  extraPkgs ? [ ],
  maxLayers ? 70,
  bsdConf ? { },
  flake-registry ? null,
  uid ? 0,
  gid ? 0,
  uname ? "root",
  gname ? "root",
  Labels ? {
    "org.opencontainers.image.title" = "Bsd";
    "org.opencontainers.image.source" = "https://github.com/BasedLinux/bsd";
    "org.opencontainers.image.vendor" = "Bsd project";
    "org.opencontainers.image.version" = bsd.version;
    "org.opencontainers.image.description" = "Bsd container image";
  },
  Cmd ? [ (lib.getExe bashInteractive) ],
  # Default Packages
  bsd,
  bashInteractive,
  coreutils-full,
  gnutar,
  gzip,
  gnugrep,
  which,
  curl,
  less,
  wget,
  man,
  cacert,
  findutils,
  iana-etc,
  gitMinimal,
  openssh,
  # Other dependencies
  shadow,
}:
let
  defaultPkgs = [
    bsd
    bashInteractive
    coreutils-full
    gnutar
    gzip
    gnugrep
    which
    curl
    less
    wget
    man
    cacert.out
    findutils
    iana-etc
    gitMinimal
    openssh
  ] ++ extraPkgs;

  users =
    {

      root = {
        uid = 0;
        shell = lib.getExe bashInteractive;
        home = "/root";
        gid = 0;
        groups = [ "root" ];
        description = "System administrator";
      };

      nobody = {
        uid = 65534;
        shell = lib.getExe' shadow "nologin";
        home = "/var/empty";
        gid = 65534;
        groups = [ "nobody" ];
        description = "Unprivileged account (don't use!)";
      };

    }
    // lib.optionalAttrs (uid != 0) {
      "${uname}" = {
        uid = uid;
        shell = lib.getExe bashInteractive;
        home = "/home/${uname}";
        gid = gid;
        groups = [ "${gname}" ];
        description = "Bsd user";
      };
    }
    // lib.listToAttrs (
      map (n: {
        name = "bsdbld${toString n}";
        value = {
          uid = 30000 + n;
          gid = 30000;
          groups = [ "bsdbld" ];
          description = "Bsd build user ${toString n}";
        };
      }) (lib.lists.range 1 32)
    );

  groups =
    {
      root.gid = 0;
      bsdbld.gid = 30000;
      nobody.gid = 65534;
    }
    // lib.optionalAttrs (gid != 0) {
      "${gname}".gid = gid;
    };

  userToPasswd = (
    k:
    {
      uid,
      gid ? 65534,
      home ? "/var/empty",
      description ? "",
      shell ? "/bin/false",
      groups ? [ ],
    }:
    "${k}:x:${toString uid}:${toString gid}:${description}:${home}:${shell}"
  );
  passwdContents = (lib.concatStringsSep "\n" (lib.attrValues (lib.mapAttrs userToPasswd users)));

  userToShadow = k: { ... }: "${k}:!:1::::::";
  shadowContents = (lib.concatStringsSep "\n" (lib.attrValues (lib.mapAttrs userToShadow users)));

  # Map groups to members
  # {
  #   group = [ "user1" "user2" ];
  # }
  groupMemberMap = (
    let
      # Create a flat list of user/group mappings
      mappings = (
        builtins.foldl' (
          acc: user:
          let
            groups = users.${user}.groups or [ ];
          in
          acc
          ++ map (group: {
            inherit user group;
          }) groups
        ) [ ] (lib.attrNames users)
      );
    in
    (builtins.foldl' (
      acc: v:
      acc
      // {
        ${v.group} = acc.${v.group} or [ ] ++ [ v.user ];
      }
    ) { } mappings)
  );

  groupToGroup =
    k:
    { gid }:
    let
      members = groupMemberMap.${k} or [ ];
    in
    "${k}:x:${toString gid}:${lib.concatStringsSep "," members}";
  groupContents = (lib.concatStringsSep "\n" (lib.attrValues (lib.mapAttrs groupToGroup groups)));

  defaultBsdConf = {
    sandbox = "false";
    build-users-group = "bsdbld";
    trusted-public-keys = [ "cache.basedlinux.org-1:6NCHdD59X431o0gWypbMrAURkbJ16ZPMQFGspcDShjY=" ];
  };

  bsdConfContents =
    (lib.concatStringsSep "\n" (
      lib.mapAttrsToList (
        n: v:
        let
          vStr = if builtins.isList v then lib.concatStringsSep " " v else v;
        in
        "${n} = ${vStr}"
      ) (defaultBsdConf // bsdConf)
    ))
    + "\n";

  userHome = if uid == 0 then "/root" else "/home/${uname}";

  baseSystem =
    let
      bsdpkgs = pkgs.path;
      channel = runCommand "channel-bsdos" { inherit bundleBsdpkgs; } ''
        mkdir $out
        if [ "$bundleBsdpkgs" ]; then
          ln -s ${
            builtins.path {
              path = bsdpkgs;
              name = "source";
            }
          } $out/bsdpkgs
          echo "[]" > $out/manifest.bsd
        fi
      '';
      rootEnv = buildPackages.buildEnv {
        name = "root-profile-env";
        paths = defaultPkgs;
      };
      manifest = buildPackages.runCommand "manifest.bsd" { } ''
        cat > $out <<EOF
        [
        ${lib.concatStringsSep "\n" (
          builtins.map (
            drv:
            let
              outputs = drv.outputsToInstall or [ "out" ];
            in
            ''
              {
                ${lib.concatStringsSep "\n" (
                  builtins.map (output: ''
                    ${output} = { outPath = "${lib.getOutput output drv}"; };
                  '') outputs
                )}
                outputs = [ ${lib.concatStringsSep " " (builtins.map (x: "\"${x}\"") outputs)} ];
                name = "${drv.name}";
                outPath = "${drv}";
                system = "${drv.system}";
                type = "derivation";
                meta = { };
              }
            ''
          ) defaultPkgs
        )}
        ]
        EOF
      '';
      profile = buildPackages.runCommand "user-environment" { } ''
        mkdir $out
        cp -a ${rootEnv}/* $out/
        ln -s ${manifest} $out/manifest.bsd
      '';
      flake-registry-path =
        if (flake-registry == null) then
          null
        else if (builtins.readFileType (toString flake-registry)) == "directory" then
          "${flake-registry}/flake-registry.json"
        else
          flake-registry;
    in
    runCommand "base-system"
      {
        inherit
          passwdContents
          groupContents
          shadowContents
          bsdConfContents
          ;
        passAsFile = [
          "passwdContents"
          "groupContents"
          "shadowContents"
          "bsdConfContents"
        ];
        allowSubstitutes = false;
        preferLocalBuild = true;
      }
      (
        ''
          env
          set -x
          mkdir -p $out/etc

          mkdir -p $out/etc/ssl/certs
          ln -s /bsd/var/bsd/profiles/default/etc/ssl/certs/ca-bundle.crt $out/etc/ssl/certs

          cat $passwdContentsPath > $out/etc/passwd
          echo "" >> $out/etc/passwd

          cat $groupContentsPath > $out/etc/group
          echo "" >> $out/etc/group

          cat $shadowContentsPath > $out/etc/shadow
          echo "" >> $out/etc/shadow

          mkdir -p $out/usr
          ln -s /bsd/var/bsd/profiles/share $out/usr/

          mkdir -p $out/bsd/var/bsd/gcroots

          mkdir $out/tmp

          mkdir -p $out/var/tmp

          mkdir -p $out/etc/bsd
          cat $bsdConfContentsPath > $out/etc/bsd/bsd.conf

          mkdir -p $out${userHome}
          mkdir -p $out/bsd/var/bsd/profiles/per-user/${uname}

          ln -s ${profile} $out/bsd/var/bsd/profiles/default-1-link
          ln -s /bsd/var/bsd/profiles/default-1-link $out/bsd/var/bsd/profiles/default

          ln -s ${channel} $out/bsd/var/bsd/profiles/per-user/${uname}/channels-1-link
          ln -s /bsd/var/bsd/profiles/per-user/${uname}/channels-1-link $out/bsd/var/bsd/profiles/per-user/${uname}/channels

          mkdir -p $out${userHome}/.bsd-defexpr
          ln -s /bsd/var/bsd/profiles/per-user/${uname}/channels $out${userHome}/.bsd-defexpr/channels
          echo "${channelURL} ${channelName}" > $out${userHome}/.bsd-channels

          mkdir -p $out/bin $out/usr/bin
          ln -s ${lib.getExe' coreutils-full "env"} $out/usr/bin/env
          ln -s ${lib.getExe bashInteractive} $out/bin/sh

        ''
        + (lib.optionalString (flake-registry-path != null) ''
          bsdCacheDir="${userHome}/.cache/bsd"
          mkdir -p $out$bsdCacheDir
          globalFlakeRegistryPath="$bsdCacheDir/flake-registry.json"
          ln -s ${flake-registry-path} $out$globalFlakeRegistryPath
          mkdir -p $out/bsd/var/bsd/gcroots/auto
          rootName=$(${lib.getExe' bsd "bsd"} --extra-experimental-features bsd-command hash file --type sha1 --base32 <(echo -n $globalFlakeRegistryPath))
          ln -s $globalFlakeRegistryPath $out/bsd/var/bsd/gcroots/auto/$rootName
        '')
      );

in
dockerTools.buildLayeredImageWithBsdDb {

  inherit
    name
    tag
    maxLayers
    uid
    gid
    uname
    gname
    ;

  contents = [ baseSystem ];

  extraCommands = ''
    rm -rf bsd-support
    ln -s /bsd/var/bsd/profiles bsd/var/bsd/gcroots/profiles
  '';
  fakeRootCommands = ''
    chmod 1777 tmp
    chmod 1777 var/tmp
    chown -R ${toString uid}:${toString gid} .${userHome}
    chown -R ${toString uid}:${toString gid} bsd
  '';

  config = {
    inherit Cmd Labels;
    User = "${toString uid}:${toString gid}";
    Env = [
      "USER=${uname}"
      "PATH=${
        lib.concatStringsSep ":" [
          "${userHome}/.bsd-profile/bin"
          "/bsd/var/bsd/profiles/default/bin"
          "/bsd/var/bsd/profiles/default/sbin"
        ]
      }"
      "MANPATH=${
        lib.concatStringsSep ":" [
          "${userHome}/.bsd-profile/share/man"
          "/bsd/var/bsd/profiles/default/share/man"
        ]
      }"
      "SSL_CERT_FILE=/bsd/var/bsd/profiles/default/etc/ssl/certs/ca-bundle.crt"
      "GIT_SSL_CAINFO=/bsd/var/bsd/profiles/default/etc/ssl/certs/ca-bundle.crt"
      "NIX_SSL_CERT_FILE=/bsd/var/bsd/profiles/default/etc/ssl/certs/ca-bundle.crt"
      "NIX_PATH=/bsd/var/bsd/profiles/per-user/${uname}/channels:${userHome}/.bsd-defexpr/channels"
    ];
  };

}
