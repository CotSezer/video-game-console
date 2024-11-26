#!/bin/bash

# Set paths
DISK_IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"
LOOP_DEVICE=""

# Create mount directory if it doesn't exist
if [ ! -d "$MOUNT_POINT" ]; then
    mkdir $MOUNT_POINT
fi

# Attach the disk image to a loop device
LOOP_DEVICE=$(losetup --find --show $DISK_IMAGE)

# Mount the loop device
mount $LOOP_DEVICE $MOUNT_POINT

echo "Disk image mounted at $MOUNT_POINT."

