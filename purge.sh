#!/bin/bash

# Variables
IMAGE_NAME="storage_vgc.img"
MOUNT_DIR="mount"
LINK_NAME="storage_vgc_device"

# Unmount and detach using terminate.sh
./terminate.sh

# Remove the image file
if [ -f $IMAGE_NAME ]; then
    echo "Removing $IMAGE_NAME..."
    rm -f $IMAGE_NAME
fi

# Force remove the mount directory if necessary
if [ -d $MOUNT_DIR ]; then
    echo "Force removing mount directory $MOUNT_DIR..."
    sudo umount -l $MOUNT_DIR 2>/dev/null
    rmdir $MOUNT_DIR || echo "Could not remove $MOUNT_DIR. It may still be in use."
fi

echo "Storage purged."

