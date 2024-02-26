sudo apt install qemu-system-aarch64 -y
sudo apt install gcc-aarch64-linux-gnu -y
mkdir build
cd build
mkdir boot core debug drivers fs lib mm shell sse
cd ../guests
rm -rf deltaOS
git clone https://www.github.com/yuvraj1803/deltaOS.git
cd deltaOS
chmod +x install.sh
./install.sh