#!/bin/bash

# ============================
# 1. Configure IPv4
# ============================
sudo ip addr add 10.0.2.100/24 dev eth0
sudo ip route add default via 10.0.2.1

# ============================
# 2. Configure IPv6
# ============================
sudo ip addr add 2001:db8:10::100/64 dev eth0
sudo ip -6 route add default via 2001:db8:10::1

# ============================
# 3. Bring interface up
# ============================
sudo ip link set eth0 up

# starting server
python3 -m http.server 8000 --bind ::