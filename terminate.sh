#!/bin/bash

# Variables
MOUNT_DIR="mount_storage"
LINK_NAME="storage_vgc_device"
LOOP_DEVICE=$(readlink $LINK_NAME)

# Unmount the image
if mountpoint -q $MOUNT_DIR; then
    echo "Unmounting $MOUNT_DIR..."
    sudo umount $MOUNT_DIR
fi

# Detach the loop device
if [ -n "$LOOP_DEVICE" ]; then
    echo "Detaching $LOOP_DEVICE..."
    sudo losetup -d $LOOP_DEVICE
fi

# Remove symbolic link
if [ -L $LINK_NAME ]; then
    echo "Removing symbolic link $LINK_NAME..."
    rm -f $LINK_NAME
fi

echo "Storage terminated."

