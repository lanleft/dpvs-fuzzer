#!/bin/sh

# create a bridge
sudo ip link add name br0 type bridge
sudo ip addr add 10.0.2.1/24 dev br0
sudo ip link set br0 up

# create tap interfaces
sudo ip tuntap add mode tap tap0
sudo ip tuntap add mode tap tap1
sudo ip link set tap0 master br0
sudo ip link set tap1 master br0
sudo ip link set tap0 up
sudo ip link set tap1 up


# Configure NAT with iptables
sudo iptables -t nat -A POSTROUTING -s 10.0.2.0/24 -o enp0s31f6 -j MASQUERADE

# Allow forwarding in iptables
sudo iptables -A FORWARD -i br0 -o enp0s31f6 -j ACCEPT
sudo iptables -A FORWARD -i enp0s31f6 -o br0 -m state --state RELATED,ESTABLISHED -j ACCEPT

# allow forwarding for the VM subnet on host 
sudo iptables -A FORWARD -s 10.0.2.0/24 -d 10.0.2.0/24 -j ACCEPT
