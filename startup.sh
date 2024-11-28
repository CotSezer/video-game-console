#!/bin/bash

# Set paths
DISK_IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"
DEVICE_FILE="<device-file>"  # Name of the symbolic link

# Create mount directory if it doesn't exist
if [ ! -d "$MOUNT_POINT" ]; then
    mkdir $MOUNT_POINT
fi

# Mount the disk image directly using loop (no need for losetup)
mount -o loop $DISK_IMAGE $MOUNT_POINT

# Create the necessary directories inside the mounted image if they don't exist
mkdir -p $MOUNT_POINT/bin
mkdir -p $MOUNT_POINT/src

# Create a symbolic link to the device (optional)
if [ ! -L "$DEVICE_FILE" ]; then
    ln -s $MOUNT_POINT $DEVICE_FILE
    echo "Created symbolic link $DEVICE_FILE."
fi

# Copy the executables to the mounted image
cp bin/* $MOUNT_POINT/bin/

# Set executable permissions for the copied executables
chmod +x $MOUNT_POINT/bin/*

echo "Disk image mounted at $MOUNT_POINT."
echo "Executables copied to disk image and permissions set."
