INC = ./include
LIB = ./lib/include
TOOLCHAIN=aarch64-linux-gnu-
CC = $(TOOLCHAIN)gcc
AS = $(TOOLCHAIN)as
LD = $(TOOLCHAIN)ld
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump

CFLAGS = -g -ffreestanding -nostdlib -nostartfiles -Wall -I$(INC) -I$(LIB)
ASMFLAGS = -g -I$(INC)


OBJ += ./build/core/main.o
OBJ += ./build/core/delay.o
OBJ += ./build/mm/mm.o
OBJ += ./build/boot/boot.S.o
OBJ += ./build/drivers/uart.o
OBJ += ./build/drivers/sd.o
OBJ += ./build/drivers/disk.o
OBJ += ./build/lib/stdio.o
OBJ += ./build/lib/memory.o
OBJ += ./build/lib/string.o
OBJ += ./build/fs/file.o
OBJ += ./build/fs/fat16.o
OBJ += ./build/fs/parser.o

.PHONY: all
all: SD_with_guests kernel8.img

./build/core/%.o: ./core/%.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/mm/%.o: ./mm/%.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/boot/boot.S.o: ./boot/boot.S
	$(CC) $(ASMFLAGS) -c $< -o $@

./build/drivers/%.o: ./drivers/%.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/fs/%.o: ./fs/%.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/lib/%.o: ./lib/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img : $(OBJ)
	$(LD) -T linker.ld -o ./deltaV.elf $(OBJ_C) $(OBJ)
	$(OBJCOPY) ./deltaV.elf -O binary ./kernel8.img


.PHONY: run
run: 	SD_with_guests kernel8.img qemu

.PHONY: debug
debug:	SD_with_guests kernel8.img 
	qemu-system-aarch64 -M raspi3b -nographic -serial null -serial mon:stdio -m 1024 -kernel ./kernel8.img -drive file=./SD_with_guests.img,if=sd,format=raw -s -S


.PHONY: qemu
qemu: 
	qemu-system-aarch64 -M raspi3b -nographic -serial null -serial mon:stdio -m 1024 -kernel ./kernel8.img -drive file=./SD_with_guests.img,if=sd,format=raw

.PHONY: SD_with_guests
SD_with_guests:
	dd if=/dev/zero count=128 bs=1M > SD_with_guests.img
	(echo o; echo n; echo p; echo 1; echo 2048; echo 99999; echo t; echo 6; echo w; echo q) | sudo fdisk SD_with_guests.img
	mkfs.fat -F 16 SD_with_guests.img
	sudo mkdir /mnt/delta_guests
	sudo mount SD_with_guests.img /mnt/delta_guests
	sudo cp -r ./guests /mnt/delta_guests
	sudo umount /mnt/delta_guests
	sudo rmdir /mnt/delta_guests

.PHONY: clean
clean:
	rm -f *.elf
	rm -f *.img
	find ./build -name '*.o' -delete 



		
