#!/bin/bash

# Set paths
DISK_IMAGE="storage_vgc.img"
MOUNT_POINT="./mount"
DEVICE_FILE="<device-file>"  # Name of the symbolic link
LOOP_DEVICE="/dev/loop0"  # Use the loop device you just created

# Create mount directory if it doesn't exist
if [ ! -d "$MOUNT_POINT" ]; then
    mkdir $MOUNT_POINT
fi

# Mount the loop device to the mount point
sudo mount $LOOP_DEVICE $MOUNT_POINT

# Create the necessary directories inside the mounted image if they don't exist
mkdir -p $MOUNT_POINT/bin
mkdir -p $MOUNT_POINT/src

# Create a symbolic link to the device (optional)
if [ ! -L "$DEVICE_FILE" ]; then
    ln -s $LOOP_DEVICE $DEVICE_FILE
    echo "Created symbolic link $DEVICE_FILE."
fi

# Copy the executables to the mounted image
cp bin/* $MOUNT_POINT/bin/

# Set executable permissions for the copied executables
chmod +x $MOUNT_POINT/bin/*

echo "Disk image mounted at $MOUNT_POINT."
echo "Executables copied to disk image and permissions set."
