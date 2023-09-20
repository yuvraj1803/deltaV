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
OBJ += ./build/core/irq.S.o
OBJ += ./build/core/exceptions.S.o
OBJ += ./build/core/misc.S.o
OBJ += ./build/core/irq.o
OBJ += ./build/core/sched.o
OBJ += ./build/mm/mm.o
OBJ += ./build/mm/mm.S.o
OBJ += ./build/mm/paging.o
OBJ += ./build/boot/boot.S.o
OBJ += ./build/drivers/uart.o
OBJ += ./build/drivers/sd.o
OBJ += ./build/drivers/timer.o
OBJ += ./build/lib/stdio.o
OBJ += ./build/lib/memory.o
OBJ += ./build/lib/string.o
OBJ += ./build/lib/stdlib.o
OBJ += ./build/fs/ff.o
OBJ += ./build/fs/diskio.o

.PHONY: all
all: sdcard kernel8.img

./build/core/%.o: ./core/%.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/mm/%.o: ./mm/%.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/boot/%.S.o: ./boot/%.S
	$(CC) $(ASMFLAGS) -c $< -o $@

./build/core/%.S.o : ./core/%.S
	$(CC) $(ASMFLAGS) -c $< -o $@

./build/mm/%.S.o: ./mm/%.S
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
run: 	sdcard kernel8.img qemu

.PHONY: debug
debug:	sdcard kernel8.img 
	qemu-system-aarch64 -M raspi3b -nographic -serial null -serial mon:stdio -m 1024 -kernel ./kernel8.img -drive file=./sdcard.img,if=sd,format=raw -s -S

.PHONY: qemu
qemu: 
	qemu-system-aarch64 -M raspi3b -nographic -serial null -serial mon:stdio -m 1024 -kernel ./kernel8.img -drive file=./sdcard.img,if=sd,format=raw

.PHONY: sdcard
sdcard:
	sudo modprobe nbd max_part=8
	qemu-img create sdcard.img 128m
	sudo qemu-nbd -c /dev/nbd0 --format=raw sdcard.img 
	(echo o; echo n; echo p; echo 1; echo ; echo ;echo w; echo q) | sudo fdisk /dev/nbd0
	sudo mkfs.fat -F32 /dev/nbd0p1
	mkdir temp || true
	sudo mount -o user /dev/nbd0p1 temp/
	sudo cp  -r ./guests temp/
	sleep 1s
	sudo umount temp/
	rmdir temp/ || true
	sudo qemu-nbd -d /dev/nbd0
	(echo t; echo c; echo w; echo q) | sudo fdisk sdcard.img

.PHONY: objdump
objdump: kernel8.img
	$(OBJDUMP) -D deltaV.elf > deltaV.list

.PHONY: clean
clean:
	rm -f *.elf
	rm -f *.img
	find ./build -name '*.o' -delete 
	find ./build -name '*.d' -delete
	sudo qemu-nbd -d /dev/nbd0
	rm -rf *.list


		
