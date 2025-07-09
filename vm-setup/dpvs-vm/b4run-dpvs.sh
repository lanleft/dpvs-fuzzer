#!/bin/bash

# ============================
# 1. Configure IPv4
# ============================
sudo ip addr add 10.0.2.15/24 dev eth0
sudo ip route add default via 10.0.2.1

# ============================
# 2. Configure IPv6
# ============================
sudo ip addr add 2001:db8:10::15/64 dev eth0
sudo ip -6 route add default via 2001:db8:10::1

# ============================
# 3. Bring interface up
# ============================
sudo ip link set eth0 up

echo 8192 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
export PKG_CONFIG_PATH=/home/user/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu/pkgconfig/