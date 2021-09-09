#!bin/bash

mountpoint -q /mnt/iso; [ "$?" -ne "0" ] | sudo umount /mnt/iso
sudo rm -rf /mnt/iso

mountpoint -q /mnt/usb; [ "$?" -ne "0" ] | sudo umount /mnt/usb
sudo rm -rf /mnt/usb