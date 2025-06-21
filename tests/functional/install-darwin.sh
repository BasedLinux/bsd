#!/bin/sh

set -eux

cleanup() {
    PLIST="/Library/LaunchDaemons/org.nixos.nix-daemon.plist"
    if sudo launchctl list | grepQuiet bsd-daemon; then
        sudo launchctl unload "$PLIST"
    fi

    if [ -f "$PLIST" ]; then
        sudo rm /Library/LaunchDaemons/org.nixos.nix-daemon.plist
    fi

    profiles=(/etc/profile /etc/bashrc /etc/zshrc)
    for profile in "${profiles[@]}"; do
        if [ -f "${profile}.backup-before-bsd" ]; then
            sudo mv "${profile}.backup-before-bsd" "${profile}"
        fi
    done

    for file in ~/.bash_profile ~/.bash_login ~/.profile ~/.zshenv ~/.zprofile ~/.zshrc ~/.zlogin; do
        if [ -e "$file" ]; then
            cat "$file" | grep -v bsd-profile > "$file.next"
            mv "$file.next" "$file"
        fi
    done

    for i in $(seq 1 $(sysctl -n hw.ncpu)); do
        sudo /usr/bin/dscl . -delete "/Users/bsdbld$i" || true
    done
    sudo /usr/bin/dscl . -delete "/Groups/bsdbld" || true

    sudo rm -rf /etc/bsd \
         /bsd \
         /var/root/.nix-profile /var/root/.nix-defexpr /var/root/.nix-channels \
         "$HOME/.nix-profile" "$HOME/.nix-defexpr" "$HOME/.nix-channels"
}

verify() {
    set +e
    output=$(echo "bsd-shell -p bash --run 'echo toow | rev'" | bash -l)
    set -e

    test "$output" = "woot"
}

scratch=$(mktemp -d -t tmp.XXXXXXXXXX)
function finish {
    rm -rf "$scratch"
}
trap finish EXIT

# First setup Bsd
cleanup
curl -L -o install https://basedlinux.org/bsd/install
yes | bash ./install
verify


(
    set +e
    (
        echo "cd $(pwd)"
        echo bsd-build ./release.nix -A binaryTarball.x86_64-darwin
    ) | bash -l
    set -e
    cp ./result/bsd-*.tar.bz2 $scratch/bsd.tar.bz2
)

(
    cd $scratch
    tar -xf ./bsd.tar.bz2

    cd bsd-*

    set -eux

    cleanup

    yes | ./install
    verify
    cleanup

    echo -n "" | ./install
    verify
    cleanup

    sudo mkdir -p /bsd/store
    sudo touch /bsd/store/.silly-hint
    echo -n "" | ALLOW_PREEXISTING_INSTALLATION=true ./install
    verify
    test -e /bsd/store/.silly-hint

    cleanup
)
