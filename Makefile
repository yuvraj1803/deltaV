INC = ./include
BUILD = ./BUILD/
TOOLCHAIN=aarch64-linux-gnu-
CC = $(TOOLCHAIN)gcc
AS = $(TOOLCHAIN)as
LD = $(TOOLCHAIN)ld
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump

CFLAGS = -g -ffreestanding -nostdlib -nostartfiles -Wall -I$(INC)
ASMFLAGS = -g -I$(INC)


OBJ += ./build/core/main.o
OBJ += ./build/mm/mm.o
OBJ += ./build/boot/boot.S.o
OBJ += ./build/drivers/uart/uart.o

.PHONY: all
all: clean kernel8.img

./build/core/main.o: ./core/main.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/mm/mm.o: ./mm/mm.c
	$(CC) $(CFLAGS) -c $< -o $@

./build/boot/boot.S.o: ./boot/boot.S
	$(CC) $(ASMFLAGS) -c $< -o $@

./build/drivers/uart/uart.o: ./drivers/uart/uart.c
	$(CC) $(CFLAGS) -c $< -o $@


kernel8.img : $(OBJ)
	$(LD) -T linker.ld -o ./kernel8.elf $(OBJ_C) $(OBJ)
	$(OBJCOPY) ./kernel8.elf -O binary ./kernel8.img


.PHONY: clean
clean:
	rm -f kernel8.elf
	rm -f kernel8.img
	find ./build -name '*.o' -delete 



		

