#!/bin/bash

# =======================
# 1. Create bridge (br0)
# =======================
sudo ip link add name br0 type bridge
sudo ip addr add 10.0.2.1/24 dev br0              # IPv4 address
sudo ip addr add 2001:db8:10::1/64 dev br0         # IPv6 address (example range)
sudo ip link set br0 up

# =======================
# 2. Create tap interfaces (tap0 and tap1)
# =======================
sudo ip tuntap add mode tap tap0
sudo ip tuntap add mode tap tap1

# Attach taps to bridge
sudo ip link set tap0 master br0
sudo ip link set tap1 master br0

# Bring up tap interfaces
sudo ip link set tap0 up
sudo ip link set tap1 up

# =======================
# 3. Configure NAT for IPv4
# =======================
sudo sysctl -w net.ipv4.ip_forward=1

# Replace enp0s31f6 with your real physical network interface name if needed
sudo iptables -t nat -A POSTROUTING -s 10.0.2.0/24 -o enp0s31f6 -j MASQUERADE

# Allow forwarding between bridge and external network
sudo iptables -A FORWARD -i br0 -o enp0s31f6 -j ACCEPT
sudo iptables -A FORWARD -i enp0s31f6 -o br0 -m state --state RELATED,ESTABLISHED -j ACCEPT

# Allow forwarding within bridge network
sudo iptables -A FORWARD -s 10.0.2.0/24 -d 10.0.2.0/24 -j ACCEPT

# =======================
# 4. Configure NAT for IPv6 (masquerade is a bit different)
# =======================
# Enable IPv6 forwarding
sudo sysctl -w net.ipv6.conf.all.forwarding=1

# Enable NAT66 if necessary (Optional: some Linux distros support it with ip6tables)
# Normally you just route directly with IPv6, but for symmetric setup:
# sudo ip6tables -t nat -A POSTROUTING -s 2001:db8:10::/64 -o enp0s31f6 -j MASQUERADE

# Allow forwarding IPv6 packets between bridge and external network
sudo ip6tables -A FORWARD -i br0 -o enp0s31f6 -j ACCEPT
sudo ip6tables -A FORWARD -i enp0s31f6 -o br0 -m state --state RELATED,ESTABLISHED -j ACCEPT

# Allow forwarding inside bridge for IPv6
sudo ip6tables -A FORWARD -s 2001:db8:10::/64 -d 2001:db8:10::/64 -j ACCEPT
