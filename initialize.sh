#!/bin/bash

# Variables
IMAGE_NAME="storage_vgc.img"
IMAGE_SIZE="1M" # Size of the image file

# Create a new disk image
echo "Creating $IMAGE_NAME..."
dd if=/dev/zero of=$IMAGE_NAME bs=1M count=100 status=progress
echo "Formatting $IMAGE_NAME as ext4..."
mkfs.ext4 $IMAGE_NAME

echo "$IMAGE_NAME created and formatted."

