#!/bin/bash

# Variables
IMAGE_NAME="storage_vgc.img"
IMAGE_SIZE="50M" # Size of the image file

# Create a new disk image
echo "Creating $IMAGE_NAME..."
dd if=/dev/zero of=$IMAGE_NAME bs=1M count=50 status=progress
echo "Formatting $IMAGE_NAME as ext4..."
sudo mkfs.ext4 $IMAGE_NAME

echo "$IMAGE_NAME created and formatted."

