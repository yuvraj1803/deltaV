INC = ./include
LIB = ./lib/include
BUILD = ./BUILD/
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
OBJ += ./build/lib/stdio.o

.PHONY: all
all: kernel8.img

./build/core/%.o: ./core/%.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/mm/%.o: ./mm/%.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/boot/boot.S.o: ./boot/boot.S
	$(CC) $(ASMFLAGS) -c $< -o $@

./build/drivers/%.o: ./drivers/%.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/lib/stdio.o: ./lib/src/stdio.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel8.img : $(OBJ)
	$(LD) -T linker.ld -o ./deltaV.elf $(OBJ_C) $(OBJ)
	$(OBJCOPY) ./deltaV.elf -O binary ./kernel8.img


.PHONY: SD_with_guests
SD_with_guests:
	./scripts/create_SD_with_guests ./guests *.bin

.PHONY: clean
clean:
	rm -f *.elf
	rm -f *.img
	find ./build -name '*.o' -delete 



		

