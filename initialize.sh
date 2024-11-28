#!/bin/bash

# Set the name of the disk image file
DISK_IMAGE="storage_vgc.img"
DISK_SIZE="64M"  # Disk size (adjust as needed)

# Remove the existing disk image if it exists
if [ -f "$DISK_IMAGE" ]; then

    rm -f "$DISK_IMAGE"  # Force remove the existing disk image
fi

# Create a new disk image file

dd if=/dev/zero of=$DISK_IMAGE bs=1M count=64

# Format the disk image as ext4 using sudo

sudo mkfs -t ext4 $DISK_IMAGE

echo "Disk image '$DISK_IMAGE' created and formatted as ext4."
