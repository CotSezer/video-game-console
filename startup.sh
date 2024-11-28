#!/bin/bash

# Set paths
DISK_IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"
EXE_DIR="./bin"
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

# Copy all executables from the local bin directory to the mounted image's bin directory
cp $EXE_DIR/* $MOUNT_POINT/bin/

# Set execute permissions on the copied executables in the mounted image
chmod +x $MOUNT_POINT/bin/*

echo "Disk image mounted at $MOUNT_POINT."
echo "Executables copied to disk image and permissions set."
