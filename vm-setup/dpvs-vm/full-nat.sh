#!/bin/sh

### --- IPv4 Setup ---
./dpip addr add 10.0.2.105/24 dev dpdk0

# TCP service
./ipvsadm -A -t 10.0.2.105:8000 -s rr
./ipvsadm -a -t 10.0.2.105:8000 -r 10.0.2.100:8000 -b
./ipvsadm --add-laddr -z 10.0.2.15 -t 10.0.2.105:8000 -F dpdk0

# UDP service
./ipvsadm -A -u 10.0.2.105:8000 -s rr
./ipvsadm -a -u 10.0.2.105:8000 -r 10.0.2.100:8000 -b
./ipvsadm --add-laddr -z 10.0.2.15 -u 10.0.2.105:8000 -F dpdk0

### --- IPv6 Setup ---

./dpip addr add 2001:db8:10::105/64 dev dpdk0

# TCP service (IPv6)
./ipvsadm -A -6 -t [2001:db8:10::105]:8000 -s rr
./ipvsadm -a -6 -t [2001:db8:10::105]:8000 -r [2001:db8:10::100]:8000 -b
./ipvsadm --add-laddr -6 -z 2001:db8:10::15 -t [2001:db8:10::105]:8000 -F dpdk0

# UDP service (IPv6)
./ipvsadm -A -6 -u [2001:db8:10::105]:8000 -s rr
./ipvsadm -a -6 -u [2001:db8:10::105]:8000 -r [2001:db8:10::100]:8000 -b
./ipvsadm --add-laddr -6 -z 2001:db8:10::15 -u [2001:db8:10::105]:8000 -F dpdk0

### --- Show all rules ---
echo "===== IPv4 Rules ====="
./ipvsadm -ln

echo "===== IPv6 Rules ====="
./ipvsadm -ln -6
