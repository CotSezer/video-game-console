#!/bin/bash

# Set paths
DISK_IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"
DEVICE_FILE="<device-file>"  # Replace <device-file> with the symbolic link name you want
LOOP_DEVICE=""

# Check and create mount directory if it doesn't exist
if [ ! -d "$MOUNT_POINT" ]; then
    echo "Mount directory does not exist. Creating it..."
    mkdir -p $MOUNT_POINT
fi

# Attach the disk image to a loop device
LOOP_DEVICE=$(losetup --find --show $DISK_IMAGE)

# Check if the loop device is successfully created
if [ -z "$LOOP_DEVICE" ]; then
    echo "Failed to create a loop device. Exiting..."
    exit 1
fi

# Create a symbolic link to the loop device in the current directory
ln -s $LOOP_DEVICE $DEVICE_FILE

# Mount the loop device
mount $LOOP_DEVICE $MOUNT_POINT

# Check if the mount was successful
if [ $? -eq 0 ]; then
    echo "Disk image successfully mounted at $MOUNT_POINT."
    echo "Device file is available at $DEVICE_FILE (points to $LOOP_DEVICE)."
else
    echo "Failed to mount the disk image. Exiting..."
    exit 1
fi
