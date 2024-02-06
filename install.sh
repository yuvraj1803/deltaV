sudo apt install qemu-system-aarch64 -y
cd guests/
rm -rf deltaOS
git clone https://www.github.com/yuvraj1803/deltaOS.git
./guests/deltaOS/install.sh
mkdir build
mkdir build/boot
mkdir build/core
mkdir build/drivers
mkdir build/lib
mkdir build/mm
mkdir build/shell
mkdir build/debug
mkdir build/fs
mkdir build/sse
