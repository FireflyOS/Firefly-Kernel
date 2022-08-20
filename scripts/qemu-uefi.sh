#!/bin/bash

qemu-system-x86_64 -enable-kvm -M smm=off -cpu host -m 256M -boot d -no-shutdown -serial stdio -no-reboot -bios /usr/share/ovmf/OVMF.fd -cdrom ../FireflyOS_x86_64.iso