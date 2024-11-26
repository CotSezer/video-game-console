#!/bin/bash

# Set the name of the disk image file
DISK_IMAGE="storage_vgc.img"
DISK_SIZE="64M" # Disk size (adjust as needed)

# Remove existing disk image if it exists
if [ -f "$DISK_IMAGE" ]; then
    rm -f "$DISK_IMAGE"
fi

# Create the disk image file
dd if=/dev/zero of=$DISK_IMAGE bs=1M count=64

# Format the disk image as ext4
mkfs.ext4 $DISK_IMAGE

echo "Disk image created and formatted as ext4."

