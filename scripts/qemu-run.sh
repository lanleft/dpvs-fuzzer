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


# -cpu kvm64,+smep,+smap \
        # -netdev user,host=192.168.2.10,id=net1,net=192.168.2.0/24,dhcpstart=192.168.2.15 -device e1000,netdev=net1,mac=52:54:00:12:34:57 \

        # -net user,host=10.0.2.10,hostfwd=tcp:127.0.0.1:10021-:22 \
        # -net nic,model=e1000 \
        #     -s \

sudo qemu-system-x86_64 \
        -m 4G \
        -smp 4 \
        -kernel linux-6.8.12/arch/x86/boot/bzImage \
        -append "console=ttyS0 root=/dev/sda earlyprintk=serial net.ifnames=0 nokaslr" \
        -drive file=/home/lab/Desktop/side-projects/linux-setup/image/bullseye.img,format=raw \
        -netdev tap,id=net0,ifname=tap0,script=no,downscript=no \
        -device e1000,netdev=net0,mac=52:54:00:12:34:56 \
        -cpu Nehalem \
        -enable-kvm \
        -nographic \
        -pidfile vm.pid \
        -no-reboot \
        2>&1 | tee vm.log