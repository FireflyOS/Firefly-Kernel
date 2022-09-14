#!/bin/bash

qemu-system-x86_64 -M smm=off -d int -serial stdio -cpu  qemu64 -m 1G -boot d -no-shutdown  -no-reboot -cdrom ../FireflyOS_x86_64.iso