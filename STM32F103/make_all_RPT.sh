#! /bin/bash
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=Dev
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=blueDev
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=blackDev
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=blackDevTest
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=Red
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=Blue
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=Stick
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=Green
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=MapleMini
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=MapleMini_2k
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=MapleMini_ExtBd
make clean-objects
make -j3 RPT=1 Bootloader=1 SimpleCircuit=1 Platform=MapleMini_2k_ExtBd
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=Dev
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=blueDev
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=blackDev
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=blackDevTest
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=Red
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=Blue
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=Stick
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=Green
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=MapleMini
make clean-objects
make -j3 RPT=1 Bootloader=1 Platform=MapleMini_2k
make clean-objects
make SimpleCircuit=1 Platform=Dev
make clean-objects
make -j3 RPT=1 SimpleCircuit=1 Platform=blueDev
make clean-objects
make -j3 RPT=1 SimpleCircuit=1 Platform=blackDev
make clean-objects
make -j3 RPT=1 SimpleCircuit=1 Platform=blackDevTest
make clean-objects
make -j3 RPT=1 SimpleCircuit=1 Platform=Red
make clean-objects
make -j3 RPT=1 SimpleCircuit=1 Platform=Blue
make clean-objects
make -j3 RPT=1 SimpleCircuit=1 Platform=Stick
make clean-objects
make -j3 RPT=1 SimpleCircuit=1 Platform=Green
make clean-objects
make -j3 RPT=1 SimpleCircuit=1 Platform=MapleMini
make clean-objects
make -j3 RPT=1 Platform=Dev
make clean-objects
make -j3 RPT=1 Platform=blueDev
make clean-objects
make -j3 RPT=1 Platform=blackDev
make clean-objects
make -j3 RPT=1 Platform=blackDevTest
make clean-objects
make -j3 RPT=1 Platform=Red
make clean-objects
make -j3 RPT=1 Platform=Blue
make clean-objects
make -j3 RPT=1 Platform=Stick
make clean-objects
make -j3 RPT=1 Platform=Green
make clean-objects
make -j3 RPT=1 Platform=MapleMini
make clean-objects
rm *.elf
rm *.map
