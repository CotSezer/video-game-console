#!/bin/bash

# Variables
MOUNT_DIR="mount5"
LINK_NAME="storage_vgc_device"

# Resolve the loop device from the symbolic link
if [ -L "$LINK_NAME" ]; then
    LOOP_DEVICE=$(readlink -f "$LINK_NAME")
else
    LOOP_DEVICE=$(losetup -j storage_vgc.img | cut -d':' -f1)
fi

# Unmount the mount directory if it is mounted
if mountpoint -q "$MOUNT_DIR"; then
    echo "Unmounting $MOUNT_DIR..."
    sudo umount -l "$MOUNT_DIR"
    if [ $? -eq 0 ]; then
        echo "Successfully unmounted $MOUNT_DIR."
    else
        echo "Failed to unmount $MOUNT_DIR."
        exit 1
    fi
else
    echo "$MOUNT_DIR is not mounted."
fi

# Detach the loop device
if [ -n "$LOOP_DEVICE" ] && losetup | grep -q "$LOOP_DEVICE"; then
    echo "Detaching loop device $LOOP_DEVICE..."
    sudo losetup -d "$LOOP_DEVICE"
    if [ $? -eq 0 ]; then
        echo "Successfully detached $LOOP_DEVICE."
    else
        echo "Failed to detach $LOOP_DEVICE."
        exit 1
    fi
else
    echo "No active loop device found for $LINK_NAME."
fi

# Remove the symbolic link
if [ -L "$LINK_NAME" ]; then
    echo "Removing symbolic link $LINK_NAME..."
    rm -f "$LINK_NAME"
    if [ $? -eq 0 ]; then
        echo "Successfully removed $LINK_NAME."
    else
        echo "Failed to remove symbolic link $LINK_NAME."
        exit 1
    fi
else
    echo "Symbolic link $LINK_NAME does not exist."
fi

echo "Storage terminated."
