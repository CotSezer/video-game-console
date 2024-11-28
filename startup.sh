#!/bin/bash

# Set paths
DISK_IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"
DEVICE_FILE="<device-file>"  # Name of the symbolic link
LOOP_DEVICE=""

# Create mount directory if it doesn't exist
if [ ! -d "$MOUNT_POINT" ]; then
    mkdir $MOUNT_POINT
fi

# Mount the disk image directly (no need for losetup)
mount -o loop $DISK_IMAGE $MOUNT_POINT

# Create a symbolic link to the device (optional, if you want it)
if [ ! -L "$DEVICE_FILE" ]; then
    ln -s $MOUNT_POINT $DEVICE_FILE
    echo "Created symbolic link $DEVICE_FILE."
fi

echo "Disk image mounted at $MOUNT_POINT."
