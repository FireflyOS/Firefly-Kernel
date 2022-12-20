#!/bin/bash

qemu-system-x86_64 -smp 4 -enable-kvm -M smm=off -cpu host -m 8G -boot d -no-shutdown -serial stdio -no-reboot -bios /usr/share/ovmf/OVMF.fd -cdrom ../FireflyOS_x86_64.iso
