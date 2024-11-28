#!/bin/bash

# Set paths
DISK_IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"
DEVICE_FILE="<device-file>"  # The symbolic link created by startup.sh
LOOP_DEVICE=$(losetup -j $DISK_IMAGE | awk -F: '{print $1}')

# Unmount the mount point
if mountpoint -q $MOUNT_POINT; then
    umount $MOUNT_POINT
    echo "Disk image unmounted from $MOUNT_POINT."
else
    echo "Mount point $MOUNT_POINT is not mounted."
fi

# Detach the loop device
if [ -n "$LOOP_DEVICE" ]; then
    losetup -d $LOOP_DEVICE
    echo "Loop device $LOOP_DEVICE detached."
else
    echo "No loop device found associated with the disk image."
fi

# Remove the symbolic link
if [ -L "$DEVICE_FILE" ]; then
    rm -f "$DEVICE_FILE"
    echo "Removed symbolic link $DEVICE_FILE."
else
    echo "Symbolic link $DEVICE_FILE not found."
fi

# Optionally, remove the mount directory (if you want to clean up)
if [ -d "$MOUNT_POINT" ]; then
    rmdir $MOUNT_POINT
    echo "Removed mount directory $MOUNT_POINT."
else
    echo "Mount directory $MOUNT_POINT does not exist."
fi

# Remove the disk image file
if [ -f "$DISK_IMAGE" ]; then
    rm -f "$DISK_IMAGE"
    echo "Removed disk image file $DISK_IMAGE."
else
    echo "Disk image file $DISK_IMAGE not found."
fi

echo "Purge completed. All resources cleaned up, and disk image removed."
