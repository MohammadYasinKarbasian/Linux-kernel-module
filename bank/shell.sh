#!/bin/bash
make
gcc client.c
sudo insmod iutmodule.ko
#MAJOR = echo $(sudo dmesg | tail -1 | cut -d ":" -f 2)
#edcho $MAJOR
major=$(sudo dmesg | tail -1 | cut -d ":" -f 2)
echo "what is your device name?"
device_name=$1
transaction=$2
echo $device_name
echo $transaction
sudo mknod /dev/iutnode c "$major" 0
sudo ./a.out $device_name $transaction
sudo rmmod iutmodule.ko
sudo rm /dev/iutnode
make clean
sudo rm a.out
