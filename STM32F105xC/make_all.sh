#! /bin/bash
make clean-objects
make -j3 Bootloader=1 SimpleCircuit_F105=1
#make clean-objects
#make -j3 Bootloader=1
#make clean-objects
#make -j3 SimpleCircuit_F105=1
#make clean-objects
#make -j3
make clean-objects
rm *.elf
rm *.map
