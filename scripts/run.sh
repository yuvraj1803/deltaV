make 
#make SD_with_guests

qemu-system-aarch64 -M raspi3b -nographic -serial null -serial mon:stdio -m 1024 -kernel ./kernel8.img -drive file=./SD_with_guests.img,if=sd,format=raw
