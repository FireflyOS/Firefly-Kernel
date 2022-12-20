#!/bin/bash

qemu-system-x86_64 -smp 4 -enable-kvm -cpu host -m 8G -serial stdio -boot d -no-shutdown  -no-reboot -cdrom ../FireflyOS_x86_64.iso
