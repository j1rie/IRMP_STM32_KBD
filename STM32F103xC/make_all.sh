#! /bin/bash
make -j5 Bootloader=1 SimpleCircuit=1 Platform=blueDev
make clean-objects
make -j5 Bootloader=1 Platform=blueDev
make clean-objects
make -j5 SimpleCircuit=1 Platform=blueDev
make clean-objects
make -j5 Platform=blueDev
make clean-objects
rm *.elf
rm *.map
