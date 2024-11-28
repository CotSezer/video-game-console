#!/bin/bash

# Set the name of the disk image file
DISK_IMAGE="storage_vgc.img"
DISK_SIZE="64M"  # Disk size (adjust as needed)

# Remove the existing disk image if it exists
if [ -f "$DISK_IMAGE" ]; then
    echo "Existing disk image found. Removing it..."
    rm -f "$DISK_IMAGE"  # Force remove the existing disk image
fi

# Create a new disk image file
echo "Creating a new disk image of size $DISK_SIZE..."
dd if=/dev/zero of=$DISK_IMAGE bs=1M count=64 status=progress

# Format the disk image as ext4 using sudo
echo "Formatting the disk image as ext4..."
sudo mkfs -t ext4 $DISK_IMAGE

echo "Disk image created, overridden, and formatted as ext4."
