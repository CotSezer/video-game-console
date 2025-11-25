# VirtualBox Shared Folder Setup (macOS Apple Silicon → Debian Guest)

This guide provides a clean, ordered sequence for enabling shared folders on **VirtualBox 7.0+** running on **macOS (Apple Silicon)** with a **Debian Linux guest**. ARM Guest Additions are still experimental, so some manual steps are required.

---

## 1. Install VirtualBox Guest Additions (ARM)

Shared folders require Guest Additions to be installed inside the Debian guest.

### 1.1 Update packages

```
sudo apt update
sudo apt install build-essential dkms linux-headers-$(uname -r)
```

### 1.2 Insert the Guest Additions ISO

VirtualBox (macOS host) menu:

```
Devices → Insert Guest Additions CD image…
```

### 1.3 Locate the mounted ISO

```
ls /media
```

You should see entries like:

```
cdrom
cdrom0
...
```

List contents to confirm:

```
ls /media/cdrom0
```

Expected files include (also check other entires ex: cdrom):

```
VBoxLinuxAdditions.run
VBoxWindowsAdditions.exe
VBoxLinuxAdditions-arm64.run
```

### 1.4 Run the ARM installer

```
sudo /media/cdrom0/VBoxLinuxAdditions-arm64.run
```

If you get *permission denied*:

```
sudo chmod +x /media/cdrom0/VBoxLinuxAdditions-arm64.run
sudo /media/cdrom0/VBoxLinuxAdditions-arm64.run
```

If it still fails, copy to `/tmp` and run:

```
sudo cp /media/cdrom0/VBoxLinuxAdditions-arm64.run /tmp/
chmod +x /tmp/VBoxLinuxAdditions-arm64.run
sudo /tmp/VBoxLinuxAdditions-arm64.run
```

### 1.5 Reboot

```
sudo reboot
```

---

## 2. Create a Shared Folder on macOS

Choose or create a folder:

```
/Users/yourname/vmshared
```

Use any path or name you prefer.

---

## 3. Add the Shared Folder in VirtualBox

1. Shut down the Debian VM.
2. VirtualBox Manager → Select VM → **Settings**
3. Go to **Shared Folders**
4. Click the **+** icon
5. Set **Folder Path** to your macOS folder
6. Set **Folder Name** (ex: `vmshared`)
7. Enable:

   * **Auto-mount**

Start the VM.

---

## 4. Access the Shared Folder in Debian

Auto-mounted location:

```
/media/sf_vmshared
```

Check:

```
ls /media
ls /media/sf_vmshared
```

---

## 5. Fixing Permission Issues

If you see *Permission denied*, add your user to the VirtualBox group:

```
sudo usermod -aG vboxsf $USER
```

Reboot:

```
sudo reboot
```

---

## 6. Manual Mount (apply this section if Auto-Mount Fails otherwise ignore it)

Create a mount point:

```
sudo mkdir -p /mnt/vmshared
```

Mount the shared folder:

```
sudo mount -t vboxsf vmshared /mnt/vmshared
```

Check:

```
ls /mnt/vmshared
```

---

## 7. Optional: Create a Convenient Shortcut

Create a symlink inside your home directory:

```
ln -s /media/sf_vmshared ~/vmshared
```

This makes the folder easier to access in GUI file managers.

---

## Summary

* Install ARM Guest Additions manually
* Add a host folder in VirtualBox Settings
* Verify auto-mount or mount manually
* Fix permissions via the `vboxsf` group
* Add a symlink for convenience
* You can rename `vmshared`

This sequence ensures reliable shared-folder integration between macOS (Apple Silicon) and Debian guests under VirtualBox 7.0+.
