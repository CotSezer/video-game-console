#!/bin/bash

# Set paths
DISK_IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"

# Unmount and remove the mount directory
if mountpoint -q $MOUNT_POINT; then
    umount $MOUNT_POINT
fi

if [ -d "$MOUNT_POINT" ]; then
    rmdir $MOUNT_POINT
fi

# Remove the disk image
rm -f $DISK_IMAGE

echo "Disk image and mount directory cleaned up."

