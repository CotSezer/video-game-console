#!/bin/bash

# Variables
IMAGE_NAME="storage_vgc.img"
MOUNT_DIR="mount"
LINK_NAME="storage_vgc_device"

# Ensure losetup command is available
if ! command -v losetup &> /dev/null; then
    echo "Error: 'losetup' command not found. Please install util-linux."
    exit 1
fi

# Create mount directory if it doesn't exist
if [ ! -d $MOUNT_DIR ]; then
    mkdir $MOUNT_DIR
    echo "Created mount directory $MOUNT_DIR."
fi

# Find or attach the image to a loop device
LOOP_DEVICE=$(sudo losetup -f) # Use sudo for losetup

if [ -z "$LOOP_DEVICE" ]; then
    echo "Error: No available loop devices."
    exit 1
fi

# Attach the image to the loop device
echo "Attaching $IMAGE_NAME to $LOOP_DEVICE..."
sudo losetup $LOOP_DEVICE $IMAGE_NAME

# Create a symbolic link
ln -sf $LOOP_DEVICE $LINK_NAME

# Mount the loop device
echo "Mounting $LOOP_DEVICE to $MOUNT_DIR..."
sudo mount $LOOP_DEVICE $MOUNT_DIR

# Verify mount success
if [ $? -eq 0 ]; then
    echo "$IMAGE_NAME is mounted and ready to use."
else
    echo "Error: Failed to mount $IMAGE_NAME."
    sudo losetup -d $LOOP_DEVICE # Detach on failure
    rm -f $LINK_NAME
    exit 1
fi

