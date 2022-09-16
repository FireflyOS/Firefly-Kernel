#!/bin/bash

qemu-system-x86_64 -enable-kvm -cpu host -m 1G -serial stdio -boot d -no-shutdown  -no-reboot -cdrom ../FireflyOS_x86_64.iso
