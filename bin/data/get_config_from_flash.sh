arm-none-eabi-gdb ~/Documents/Github/alta-firmware/firmware/build/alta-r2.elf --silent --batch -ex "target remote localhost:3333" -ex "dump memory flash_sector_11 0x080E0000 0x08100000"
