# Uninstalling Bsd

## Multi User

Removing a [multi-user installation](./installing-binary.md#multi-user-installation) depends on the operating system.

### Linux

If you are on Linux with systemd:

1. Remove the Bsd daemon service:

   ```console
   sudo systemctl stop bsd-daemon.service
   sudo systemctl disable bsd-daemon.socket bsd-daemon.service
   sudo systemctl daemon-reload
   ```

Remove files created by Bsd:

```console
sudo rm -rf /etc/bsd /etc/profile.d/bsd.sh /etc/tmpfiles.d/bsd-daemon.conf /bsd ~root/.nix-channels ~root/.nix-defexpr ~root/.nix-profile ~root/.cache/bsd
```

Remove build users and their group:

```console
for i in $(seq 1 32); do
  sudo userdel bsdbld$i
done
sudo groupdel bsdbld
```

There may also be references to Bsd in

- `/etc/bash.bashrc`
- `/etc/bashrc`
- `/etc/profile`
- `/etc/zsh/zshrc`
- `/etc/zshrc`

which you may remove.

### macOS

> **Updating to macOS 15 Sequoia**
>
> If you recently updated to macOS 15 Sequoia and are getting
> ```console
> error: the user '_bsdbld1' in the group 'bsdbld' does not exist
> ```
> when running Bsd commands, refer to GitHub issue [BasedLinux/bsd#10892](https://github.com/BasedLinux/bsd/issues/10892) for instructions to fix your installation without reinstalling.

1. If system-wide shell initialisation files haven't been altered since installing Bsd, use the backups made by the installer:

   ```console
   sudo mv /etc/zshrc.backup-before-bsd /etc/zshrc
   sudo mv /etc/bashrc.backup-before-bsd /etc/bashrc
   sudo mv /etc/bash.bashrc.backup-before-bsd /etc/bash.bashrc
   ```

   Otherwise, edit `/etc/zshrc`, `/etc/bashrc`, and `/etc/bash.bashrc` to remove the lines sourcing `bsd-daemon.sh`, which should look like this:

   ```bash
   # Bsd
   if [ -e '/bsd/var/bsd/profiles/default/etc/profile.d/bsd-daemon.sh' ]; then
     . '/bsd/var/bsd/profiles/default/etc/profile.d/bsd-daemon.sh'
   fi
   # End Bsd
   ```

2. Stop and remove the Bsd daemon services:

   ```console
   sudo launchctl unload /Library/LaunchDaemons/org.nixos.nix-daemon.plist
   sudo rm /Library/LaunchDaemons/org.nixos.nix-daemon.plist
   sudo launchctl unload /Library/LaunchDaemons/org.nixos.darwin-store.plist
   sudo rm /Library/LaunchDaemons/org.nixos.darwin-store.plist
   ```

   This stops the Bsd daemon and prevents it from being started next time you boot the system.

3. Remove the `bsdbld` group and the `_bsdbuildN` users:

   ```console
   sudo dscl . -delete /Groups/bsdbld
   for u in $(sudo dscl . -list /Users | grep _bsdbld); do sudo dscl . -delete /Users/$u; done
   ```

   This will remove all the build users that no longer serve a purpose.

4. Edit fstab using `sudo vifs` to remove the line mounting the Bsd Store volume on `/bsd`, which looks like

   ```
   UUID=<uuid> /bsd apfs rw,noauto,nobrowse,suid,owners
   ```
   or

   ```
   LABEL=Bsd\040Store /bsd apfs rw,nobrowse
   ```

   by setting the cursor on the respective line using the arrow keys, and pressing `dd`, and then `:wq` to save the file.

   This will prevent automatic mounting of the Bsd Store volume.

5. Edit `/etc/synthetic.conf` to remove the `bsd` line.
   If this is the only line in the file you can remove it entirely:

   ```bash
   if [ -f /etc/synthetic.conf ]; then
     if [ "$(cat /etc/synthetic.conf)" = "bsd" ]; then
       sudo rm /etc/synthetic.conf
     else
       sudo vi /etc/synthetic.conf
     fi
   fi
   ```

   This will prevent the creation of the empty `/bsd` directory.

6. Remove the files Bsd added to your system, except for the store:

   ```console
   sudo rm -rf /etc/bsd /var/root/.nix-profile /var/root/.nix-defexpr /var/root/.nix-channels ~/.nix-profile ~/.nix-defexpr ~/.nix-channels
   ```


7. Remove the Bsd Store volume:

   ```console
   sudo diskutil apfs deleteVolume /bsd
   ```

   This will remove the Bsd Store volume and everything that was added to the store.

   If the output indicates that the command couldn't remove the volume, you should make sure you don't have an _unmounted_ Bsd Store volume.
   Look for a "Bsd Store" volume in the output of the following command:

   ```console
   diskutil list
   ```

   If you _do_ find a "Bsd Store" volume, delete it by running `diskutil apfs deleteVolume` with the store volume's `diskXsY` identifier.

   If you get an error that the volume is in use by the kernel, reboot and immediately delete the volume before starting any other process.

> **Note**
>
> After you complete the steps here, you will still have an empty `/bsd` directory.
> This is an expected sign of a successful uninstall.
> The empty `/bsd` directory will disappear the next time you reboot.
>
> You do not have to reboot to finish uninstalling Bsd.
> The uninstall is complete.
> macOS (Catalina+) directly controls root directories, and its read-only root will prevent you from manually deleting the empty `/bsd` mountpoint.

## Single User

To remove a [single-user installation](./installing-binary.md#single-user-installation) of Bsd, run:

```console
rm -rf /bsd ~/.nix-channels ~/.nix-defexpr ~/.nix-profile
```
You might also want to manually remove references to Bsd from your `~/.profile`.
