#!/bin/sh

# sudo qemu-system-x86_64 -s \
# 	-m 4G \
# 	-smp 4 \
# 	-kernel linux/arch/x86/boot/bzImage \
# 	-append "console=ttyS0 root=/dev/sda earlyprintk=serial net.ifnames=0" \
# 	-drive file=/home/lab/Desktop/side-projects/linux-setup/image/bullseye.img,format=raw \
# 	-net user,host=10.0.2.10,hostfwd=tcp:127.0.0.1:10021-:2222 \
# 	-net nic,model=e1000 \
# 	-enable-kvm \
# 	-nographic \
# 	-pidfile vm.pid \
# 	2>&1 | tee vm.log


sudo qemu-system-x86_64 \
        -m 4G \
        -smp 4 \
        -kernel ./bzImage2 \
        -append "console=ttyS0 root=/dev/sda earlyprintk=serial net.ifnames=0 nokaslr" \
        -drive file=/home/lab/Desktop/side-projects/linux-setup/rs-qemu/rs-bullseye.img,format=raw \
        -netdev tap,id=net0,ifname=tap1,script=no,downscript=no \
        -device e1000,netdev=net0,mac=52:54:00:12:34:57 \
        -cpu Nehalem \
        -enable-kvm \
        -nographic \
        -pidfile vm.pid \
        -no-reboot \
        2>&1 | tee vm.log