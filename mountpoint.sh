#!/bin/bash

sudo mkdir /mnt/iso
sudo mount -o loop,ro,unhide $1 /mnt/iso

usb="$2"
usb1=$(echo $usb|sed 's/.*\(........\)/\1/')
usb2=$(echo $usb1?*)
sudo umount $usb2
echo "Formate la clé USB"
sudo mkfs.vfat -n BOOTKEY $usb2
sudo mkdir /mnt/usb
sudo mount $usb2 /mnt/usb
