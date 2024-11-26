#!/bin/bash

# Set paths
DISK_IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"
LOOP_DEVICE=$(losetup -j $DISK_IMAGE | awk -F: '{print $1}')

# Unmount the mount point
umount $MOUNT_POINT

# Detach the loop device
losetup -d $LOOP_DEVICE

echo "Disk image unmounted and loop device detached."

