INC = ./include/
SRC = ./src/
BUILD = ./BUILD/
CC = $(TOOLCHAIN)gcc
AS = $(TOOLCHAIN)as
LD = $(TOOLCHAIN)ld
OBJCOPY = $(TOOLCHAIN)objcopy
OBJDUMP = $(TOOLCHAIN)objdump

CFLAGS = -g -ffreestanding -nostdlib -nostartfiles -Wall -I$(INC)
ASMFLAGS = -g -I$(INC)

.PHONY: all
all: kernel8.img

SRC_C += ./src/core/main.c
OBJ_C += ./build/src/core/main.o

SRC_ASM += ./src/boot/boot.S
OBJ_ASM += ./build/src/boot/boot.S.o

$(OBJ_C) : $(SRC_C)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_ASM) : $(SRC_ASM)
	$(AS) $(ASM_FLAGS) -c $< -o $@


kernel8.img : $(OBJ_C) $(OBJ_ASM)
	$(LD) -T $(LINKER_SCRIPT) -o ./kernel8.elf $(OBJ_C) $(OBJ_ASM)
	$(OBJCOPY) ./kernel8.elf -O binary ./kernel8.img


.PHONY: clean
clean:
	rm -f kernel8.elf
	rm -f kernel8.img
	find ./build -name '*.o' -delete 



		

