# DPVS 

Loading Balancer Network tool that Shopee is using as the proxy, handling networking traffic from external devices to internal servers

## Table of Contents

<!-- toc -->

- [Building Essential Stuffs](#building-essential-stuffs)
  * [DPDK](#dpdk)
  * [DPVS](#dpvs)
  * [Full-NAT setting up tutorial](#full-nat-setting-up-tutorial)
    + [On primary machine (that runs 2 vm)](#on-primary-machine-that-runs-2-vm)
    + [On real server machine](#on-real-server-machine)
    + [On dpvs vm](#on-dpvs-vm)
    + [On real server](#on-real-server)
- [References](#references)

<!-- tocstop -->

## Building Essential Stuffs

### DPDK

- Actually, DPVS works on the DPDK networking driver. That's why we need to build it first: 

```bash
# libs and dependencies
sudo apt update
sudo apt install -y build-essential meson ninja-build libnuma-dev libpcap-dev \
                    python3-pyelftools pkg-config zlib1g-dev libelf-dev \
                    libssl-dev libbsd-dev git

sudo apt install -y autoconf automake libtool pkg-config

# download dpdk-24.11 
cd <path-of-dpdk-24.11>
cp ../dpvs/patch/dpdk-24.11/*.patch .
patch -p1 < 0001-pdump-add-cmdline-packet-filters-for-dpdk-pdump-tool.patch
patch -p1 < 0002-debug-enable-dpdk-eal-memory-debug.patch
patch -p1 < 0003-ixgbe_flow-patch-ixgbe-fdir-rte_flow-for-dpvs.patch
patch -p1 < 0004-bonding-allow-slaves-from-different-numa-nodes.patch
patch -p1 < 0005-bonding-fix-problem-in-mode-4-dropping-multicast-pac.patch
patch -p1 < 0006-bonding-device-supports-sending-packets-from-user-sp.patch

# building
cd dpdk-24.11
mkdir dpdklib                 # user desired install folder
mkdir dpdkbuild               # user desired build folder
meson -Denable_kmods=true -Dprefix=/home/user/dpdk-24.11/dpdklib dpdkbuild --buildtype=debug # Dprefix is libs storing path
ninja -C dpdkbuild
cd dpdkbuild; ninja install
# export PKG_CONFIG_PATH=$(pwd)/../dpdklib/lib/x86_64-linux-gnu/pkgconfig/
export PKG_CONFIG_PATH=/home/user/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu/pkgconfig/

# fixing meson
sudo apt install -y python3 python3-pip
pip3 install --user --upgrade meson

# qemu size
qemu-img resize bullseye2.img 30G
sudo resize2fs /dev/sda # inside vm


```

### DPVS

- DPVS leverages DPDK to receive network packets directly and forward them to the appropricate protocol handlers in user space. Unlike the standard Linux networking stack, which operators in the kernel and depends on kernel drivers, DPVS runs entirely in userland (but networking architecture and code base are the same). It also supports huge-page memory allocation and configurable cache sizes, both of which significantly boost performance under large-scale workloads.

- Enable some debugging log and start building:

```bash
# clone dpvs from official repo

# debugging and logging
export CONFIG_DEBUG=y
export CONFIG_DPVS_NEIGH_DEBUG=y
export CONFIG_RECORD_BIG_LOOP=n
export CONFIG_DPVS_SAPOOL_DEBUG=y
export CONFIG_DPVS_IPVS_DEBUG=y
export CONFIG_DPVS_SERVICE_DEBUG=y
export CONFIG_SYNPROXY_DEBUG=y
export CONFIG_TIMER_MEASURE=n
export CONFIG_TIMER_DEBUG=n
export CONFIG_DPVS_CFG_PARSER_DEBUG=y
export CONFIG_NETIF_BONDING_DEBUG=y
export CONFIG_TC_DEBUG=y
export CONFIG_DPVS_IPVS_STATS_DEBUG=y
export CONFIG_DPVS_IP_HEADER_DEBUG=y
export CONFIG_DPVS_MBUF_DEBUG=y
export CONFIG_DPVS_IPSET_DEBUG=y
export CONFIG_NDISC_DEBUG=y
export CONFIG_MSG_DEBUG=y
export CONFIG_DPVS_MP_DEBUG=y
export CONFIG_DPVS_NETIF_DEBUG=y
export CONFIG_DPVS_ICMP_DEBUG=y
export CONFIG_DPVS_ROUTE_DEBUG=y

make -j4
make install

```

### Full-NAT setting up tutorial

- Focus on Simple Full-NAT (two-arm) as the [tutorial](https://github.com/iqiyi/dpvs/blob/master/doc/tutorial.md)
- The model is quite simple, it looks like that:

```c
/*
+---------------------+         +-------------+ 
|                     |         |             | 
|                     .15       .100          | 
|                     +--------->    rs       | 
|                     |         |             | 
|                     |         +-------------+ 
|                     |                         
|      dpvs           |                         
|                     |                         
|                     |         +--------------+
|                     |         |              |
|                     .105      .1             |
|                     <---------+  our machine |
|                     |         |              |
+---------------------+         +--------------+
 */
```


#### On primary machine (that runs 2 vm)

- Let's Two QEMU VMs talk to each other by using TAP dev

```bash
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

# inside VM1 
sudo ip addr add 10.0.2.15/24 dev eth0
sudo ip route add default via 10.0.2.1
sudo ip link set eth0 up

# inside VM2
sudo ip addr add 10.0.2.100/24 dev eth0
sudo ip route add default via 10.0.2.1
sudo ip link set eth0 up

# Configure NAT with iptables
sudo iptables -t nat -A POSTROUTING -s 10.0.2.0/24 -o enp0s31f6 -j MASQUERADE

# Allow forwarding in iptables
sudo iptables -A FORWARD -i br0 -o enp0s31f6 -j ACCEPT
sudo iptables -A FORWARD -i enp0s31f6 -o br0 -m state --state RELATED,ESTABLISHED -j ACCEPT

# allow forwarding for the VM subnet on host 
sudo iptables -A FORWARD -s 10.0.2.0/24 -d 10.0.2.0/24 -j ACCEPT

# inside VM1
ping 10.0.2.100


# clean up 
sudo ip link set br0 down
sudo ip link delete br0
sudo ip tuntap del mode tap tap0
sudo ip tuntap del mode tap tap1

```

#### On real server machine


#### On dpvs vm

```bash
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

# 4. bind devport
export PKG_CONFIG_PATH=/home/user/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu/pkgconfig/

echo 8192 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
ip link set eth0 down
/home/user/dpdk-24.11/usertools/dpdk-devbind.py -u 0000:00:03.0
/home/user/dpdk-24.11/usertools/dpdk-devbind.py -b uio_pci_generic 0000:00:03.0
/home/user/dpdk-24.11/usertools/dpdk-devbind.py --status

# 5. Run dpvs
# if we run `dpvs` only, they will take `/etc/dpvs.conf` as default configuration 
./dpvs -c [configfile]

# 6. Run full-nat set up
./full-nat.sh

```

**Normal set up**

1. Configuration 1: Full-NAT Load Balancer

```bash

$ cat setup.sh
VIP=10.0.2.105
LIP=10.0.2.15
RS=10.0.2.100

./dpip addr add ${VIP}/24 dev dpdk0
./ipvsadm -A -t ${VIP}:8000 -s rr
./ipvsadm -a -t ${VIP}:8000 -r ${RS}:8000 -b

./ipvsadm --add-laddr -z ${LIP} -t ${VIP}:8000 -F dpdk0
$
$ ./setup.sh

```

2. Configuration 2: Second way to setup Full-NAT Load Balancer

```bash
./dpip addr add 10.0.2.105/24 dev dpdk0
./ipvsadm -A -t 10.0.2.105:8000 -s rr
./ipvsadm -a -t 10.0.2.105:8000 -r 10.0.2.100:8000 -b

./ipvsadm --add-laddr -z 10.0.2.15 -t 10.0.2.105:8000 -F dpdk0

# show the rules
./ipvsadm -ln

```

3. Enable IPv6

[full-nat.sh](dpvs-vm/full-nat.sh)


*Additionally, we optimized configuration file [dpvs.conf](dpvs-vm/dpvs.conf) to run on our resources limitation*

#### On real server 

```bash
# binding network

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

```

We can use python script as ipv4+ipv6 server instead of commands: [server.py](rs-vm/server.py)

**Testing command**

```bash
debugpc at ~/Desktop/side-projects/linux-setup ❯ curl http://10.0.2.105:8000/index.html
AAAAA
```

## References

- https://blog.csdn.net/lingshengxueyuan/article/details/124062949
- How DPDK works: https://blog.csdn.net/jeawayfox/article/details/105189788
- https://github.com/iqiyi/dpvs/blob/master/doc/tutorial.md