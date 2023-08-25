make 

qemu-system-aarch64 -M raspi3b -nographic -serial null -serial mon:stdio -m 1024 -kernel ./kernel8.img -s -S
