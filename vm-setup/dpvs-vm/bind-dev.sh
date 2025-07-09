#!/bin/sh


export PKG_CONFIG_PATH=/home/user/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu/pkgconfig/

echo 8192 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
ip link set eth0 down
/home/user/dpdk-24.11/usertools/dpdk-devbind.py -u 0000:00:03.0
/home/user/dpdk-24.11/usertools/dpdk-devbind.py -b uio_pci_generic 0000:00:03.0
/home/user/dpdk-24.11/usertools/dpdk-devbind.py --status