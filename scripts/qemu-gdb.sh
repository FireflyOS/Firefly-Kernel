#!/bin/bash

qemu-system-x86_64 -enable-kvm -smp 4 -M smm=off -m 256M -boot d -no-shutdown -no-reboot -cdrom ../FireflyOS_x86_64.iso -d int -S -s
