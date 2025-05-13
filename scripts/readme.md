
# DPVS 

## Table of Contents


## 1. Introduction

**Building**

```bash
# libs and dependencies
sudo apt update
sudo apt install -y build-essential meson ninja-build libnuma-dev libpcap-dev \
                    python3-pyelftools pkg-config zlib1g-dev libelf-dev \
                    libssl-dev libbsd-dev git

sudo apt install -y autoconf automake libtool pkg-config
```

DPDK

```bash
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
meson -Denable_kmods=true -Dprefix=/home/user/dpdk-24.11/dpdklib dpdkbuild --buildtype=debug
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

DPVS

```bash

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



**Tutorial**
    - Focus on Simple Full-NAT (two-arm)
    - https://github.com/iqiyi/dpvs/blob/master/doc/tutorial.md

=> Test Full NAT load balancer

```bash
sudo ip link set eth1 up
sudo ip addr add 192.168.2.15/24 dev eth1

ip addr add 10.0.2.15/24 dev sit0

# setup hugepages
echo 8192 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
export PKG_CONFIG_PATH=/home/user/dpdk-24.11/dpdklib/lib/x86_64-linux-gnu/pkgconfig/

# change drv for eth0 to uio_pci_generic
echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
ip link set eth0 down
sudo /home/user/dpdk-24.11/usertools/dpdk-devbind.py -u 0000:00:03.0
sudo /home/user/dpdk-24.11/usertools/dpdk-devbind.py -b uio_pci_generic 0000:00:03.0
sudo /home/user/dpdk-24.11/usertools/dpdk-devbind.py --status




```

**Another Setup**

```bash

echo 1024 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
ip link set eth0 down
/home/user/dpdk-24.11/usertools/dpdk-devbind.py -u 0000:00:03.0
/home/user/dpdk-24.11/usertools/dpdk-devbind.py -b uio_pci_generic 0000:00:03.0
/home/user/dpdk-24.11/usertools/dpdk-devbind.py --status

# 
./dpip link set dpdk0 link up

```



**Let's Two QEMU VMs talk to each other**

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

**Full-NAT Load Balancer**

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

**Second way to setup Full-NAT Load Balancer**

```bash
./dpip addr add 10.0.2.105/24 dev dpdk0
./ipvsadm -A -t 10.0.2.105:8000 -s rr
./ipvsadm -a -t 10.0.2.105:8000 -r 10.0.2.100:8000 -b

./ipvsadm --add-laddr -z 10.0.2.15 -t 10.0.2.105:8000 -F dpdk0

# show the rules
./ipvsadm -ln

```

==> Done!!!

```bash
debugpc at ~/Desktop/side-projects/linux-setup ❯ curl http://10.0.2.105:8000/index.html
AAAAA
```

## Step 1: Static analysis

### ~~1. Out-of-bounds Read on csum_offset of DPDK interface~~

- On virtio_rx_offload() function, if `hdr` variable is client's payload, so we can send a large number of `csum_offset` to trigger reading out of bound. 

```c++


#define DESC_PER_CACHELINE (RTE_CACHE_LINE_SIZE / sizeof(struct vring_desc))
uint16_t
virtio_recv_pkts(void *rx_queue, struct rte_mbuf **rx_pkts, uint16_t nb_pkts)
{
	struct virtnet_rx *rxvq = rx_queue;
	struct virtqueue *vq = virtnet_rxq_to_vq(rxvq);
	struct virtio_hw *hw = vq->hw;
	struct rte_mbuf *rxm;
	uint16_t nb_used, num, nb_rx;
	uint32_t len[VIRTIO_MBUF_BURST_SZ];
	struct rte_mbuf *rcv_pkts[VIRTIO_MBUF_BURST_SZ];
	int error;
	uint32_t i, nb_enqueued;
	uint32_t hdr_size;
	struct virtio_net_hdr *hdr;

	nb_rx = 0;
	if (unlikely(hw->started == 0))
		return nb_rx;

	nb_used = virtqueue_nused(vq);

	num = likely(nb_used <= nb_pkts) ? nb_used : nb_pkts;
	if (unlikely(num > VIRTIO_MBUF_BURST_SZ))
		num = VIRTIO_MBUF_BURST_SZ;
	if (likely(num > DESC_PER_CACHELINE))
		num = num - ((vq->vq_used_cons_idx + num) % DESC_PER_CACHELINE);

	num = virtqueue_dequeue_burst_rx(vq, rcv_pkts, len, num);
	PMD_RX_LOG(DEBUG, "used:%d dequeue:%d", nb_used, num);

	nb_enqueued = 0;
	hdr_size = hw->vtnet_hdr_size;

	for (i = 0; i < num ; i++) {
		rxm = rcv_pkts[i];

		PMD_RX_LOG(DEBUG, "packet len:%d", len[i]);

		if (unlikely(len[i] < hdr_size + RTE_ETHER_HDR_LEN)) {
			PMD_RX_LOG(ERR, "Packet drop");
			nb_enqueued++;
			virtio_discard_rxbuf(vq, rxm);
			rxvq->stats.errors++;
			continue;
		}

		rxm->port = hw->port_id;
		rxm->data_off = RTE_PKTMBUF_HEADROOM;
		rxm->ol_flags = 0;
		rxm->vlan_tci = 0;

		rxm->pkt_len = (uint32_t)(len[i] - hdr_size);
		rxm->data_len = (uint16_t)(len[i] - hdr_size);

		hdr = (struct virtio_net_hdr *)((char *)rxm->buf_addr +
			RTE_PKTMBUF_HEADROOM - hdr_size);

		if (hw->vlan_strip)
			rte_vlan_strip(rxm);

		if (hw->has_rx_offload && virtio_rx_offload(rxm, hdr) < 0) {
			virtio_discard_rxbuf(vq, rxm);
			rxvq->stats.errors++;
			continue;
		}

		virtio_rx_stats_updated(rxvq, rxm);

		rx_pkts[nb_rx++] = rxm;
	}

	rxvq->stats.packets += nb_rx;

	/* Allocate new mbuf for the used descriptor */
	if (likely(!virtqueue_full(vq))) {
		uint16_t free_cnt = vq->vq_free_cnt;
		struct rte_mbuf *new_pkts[free_cnt];

		if (likely(rte_pktmbuf_alloc_bulk(rxvq->mpool, new_pkts,
						free_cnt) == 0)) {
			error = virtqueue_enqueue_recv_refill(vq, new_pkts,
					free_cnt);
			if (unlikely(error)) {
				for (i = 0; i < free_cnt; i++)
					rte_pktmbuf_free(new_pkts[i]);
			}
			nb_enqueued += free_cnt;
		} else {
			struct rte_eth_dev *dev = &rte_eth_devices[hw->port_id];
			dev->data->rx_mbuf_alloc_failed += free_cnt;
		}
	}

	if (likely(nb_enqueued)) {
		vq_update_avail_idx(vq);

		if (unlikely(virtqueue_kick_prepare(vq))) {
			virtqueue_notify(vq);
			PMD_RX_LOG(DEBUG, "Notified");
		}
	}

	return nb_rx;
}


/* Optionally fill offload information in structure */
static inline int
virtio_rx_offload(struct rte_mbuf *m, struct virtio_net_hdr *hdr)
{
	struct rte_net_hdr_lens hdr_lens;
	uint32_t hdrlen, ptype;
	int l4_supported = 0;

    //...
			off = hdr->csum_offset + hdr->csum_start;
			if (rte_pktmbuf_data_len(m) >= off + 1)
				*rte_pktmbuf_mtod_offset(m, uint16_t *,
					off) = csum;
    //...
```

Stacktrace:

```bash

sockopt_job_func
	sockopt_ctl
		lcore_job_recv_fwd
			lcore_process_packets
				netif_deliver_mbuf
					netif_rcv_mbuf

				virtio_recv_pkts

```

```c++

# netif jobs
static struct dpvs_lcore_job_array netif_jobs[NETIF_JOB_MAX] = {
    [0] = {
        .role = LCORE_ROLE_FWD_WORKER,
        .job.name = "recv_fwd",
        .job.type = LCORE_JOB_LOOP,
        .job.func = lcore_job_recv_fwd,
    },

    [1] = {
        .role = LCORE_ROLE_FWD_WORKER,
        .job.name = "xmit",
        .job.type = LCORE_JOB_LOOP,
        .job.func = lcore_job_xmit,
    },

    [2] = {
        .role = LCORE_ROLE_FWD_WORKER,
        .job.name = "timer_manage",
        .job.type = LCORE_JOB_LOOP,
        .job.func = lcore_job_timer_manage,
    },

    [3] = {
        .role = LCORE_ROLE_ISOLRX_WORKER,
        .job.name = "isol_pkt_rcv",
        .job.type = LCORE_JOB_LOOP,
        .job.func = recv_on_isol_lcore,
    },

    [4] = {
        .role = LCORE_ROLE_MASTER,
        .job.name = "timer_manage",
        .job.type = LCORE_JOB_LOOP,
        .job.func = lcore_job_timer_manage,
    },
};

```

### ~~2. Potentially OOB on DPDK virtio interface~~ 

- Before changing:

```bash

Network devices using kernel driver
===================================
0000:00:03.0 'Virtio network device 1000' if=eth0 drv=virtio-pci unused=vfio-pci,uio_pci_generic *Active*

No 'Baseband' devices detected
==============================

No 'Crypto' devices detected
============================

No 'DMA' devices detected
=========================

No 'Eventdev' devices detected
==============================

No 'Mempool' devices detected
=============================

No 'Compress' devices detected
==============================

Misc (rawdev) devices using kernel driver
=========================================
0000:00:04.0 'Virtio block device 1001' drv=virtio-pci unused=vfio-pci,uio_pci_generic 

No 'Regex' devices detected
===========================

No 'ML' devices detected
========================

CFG_FILE: Opening configuration file '/home/user/dpvs/bin/dpvs1.conf'.
CFG_FILE: log_level = DEBUG
CFG_FILE: log_file = /var/log/dpvs.log
lcore fast searching table: 
	cid:  1 -->  0
port fast searching table(port2index[cid][pid]): 
	cid:  1, pid:  0 --> index:  0
0: ./dpvs (rte_dump_stack+0x1f) [562de9a60757]
1: ./dpvs (562de3c8e000+0x5c55611) [562de98e3611]
2: ./dpvs (562de3c8e000+0x6c73c6) [562de43553c6]
3: ./dpvs (562de3c8e000+0x6e1031) [562de436f031]
4: ./dpvs (562de3c8e000+0x6ebce3) [562de4379ce3]
5: ./dpvs (562de3c8e000+0x7260e4) [562de43b40e4]
6: ./dpvs (562de3c8e000+0x7262c6) [562de43b42c6]
7: ./dpvs (eal_thread_loop+0x5c8) [562de9a42727]
8: ./dpvs (562de3c8e000+0x5dd5b3e) [562de9a63b3e]
9: /lib/x86_64-linux-gnu/libpthread.so.0 (7f7551c28000+0x7ea7) [7f7551c2fea7]
10: /lib/x86_64-linux-gnu/libc.so.6 (clone+0x3f) [7f75516daacf]

# ///

echo 8192 > /sys/devices/system/node/node0/hugepages/hugepages-2048kB/nr_hugepages
ip link set eth0 down
sudo /home/user/dpdk-24.11/usertools/dpdk-devbind.py -u 0000:00:03.0
sudo /home/user/dpdk-24.11/usertools/dpdk-devbind.py -b uio_pci_generic 0000:00:03.0
sudo /home/user/dpdk-24.11/usertools/dpdk-devbind.py --status

./dpvs -c /home/user/dpvs/bin/dpvs1.conf
./dpvs -c /home/user/dpvs/bin/dpvs2.conf
```

Debugging

```c++

────────────────────────────────────────────────────────────────────────────────
pwndbg> bt
#0  netif_rcv_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x1059b1640, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2535
#1  0x000056531d0f1874 in netif_deliver_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x1059b1640, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2453
#2  0x000056531d0f5e17 in lcore_process_packets (mbufs=0x5653235fae98 <lcore_conf+152>, cid=1 '\001', count=1, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2612
#3  0x000056531d0f9d3c in lcore_job_recv_fwd (arg=0x0) at /home/user/dpvs/src/netif.c:2656
#4  0x000056531d1340e4 in do_lcore_job (job=0x565322f83680 <netif_jobs>) at /home/user/dpvs/src/scheduler.c:165
#5  0x000056531d1342c6 in dpvs_job_loop (arg=0x0) at /home/user/dpvs/src/scheduler.c:216
#6  0x00005653227c2727 in eal_thread_loop (arg=0x1) at ../lib/eal/common/eal_common_thread.c:212
#7  0x00005653227e3b3e in eal_worker_thread_loop (arg=0x1) at ../lib/eal/linux/eal.c:867
#8  0x00007f75ffd66ea7 in start_thread (arg=<optimized out>) at pthread_create.c:477
#9  0x00007f75ff811acf in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:95


// ============================= second stacktrace =======================================
pwndbg> bt
#0  ipv4_rcv (mbuf=0x105a4ce80, port=0x10022c340) at /home/user/dpvs/src/ipv4.c:398
#1  0x000056531d0f5a0b in netif_rcv_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x105a4ce80, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2535
#2  0x000056531d0f1874 in netif_deliver_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x105a4ce80, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2453
#3  0x000056531d0f5e17 in lcore_process_packets (mbufs=0x5653235fae98 <lcore_conf+152>, cid=1 '\001', count=3, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2612
#4  0x000056531d0f9d3c in lcore_job_recv_fwd (arg=0x0) at /home/user/dpvs/src/netif.c:2656
#5  0x000056531d1340e4 in do_lcore_job (job=0x565322f83680 <netif_jobs>) at /home/user/dpvs/src/scheduler.c:165
#6  0x000056531d1342c6 in dpvs_job_loop (arg=0x0) at /home/user/dpvs/src/scheduler.c:216
#7  0x00005653227c2727 in eal_thread_loop (arg=0x1) at ../lib/eal/common/eal_common_thread.c:212
#8  0x00005653227e3b3e in eal_worker_thread_loop (arg=0x1) at ../lib/eal/linux/eal.c:867
#9  0x00007f75ffd66ea7 in start_thread (arg=<optimized out>) at pthread_create.c:477
#10 0x00007f75ff811acf in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:95


// breakpoint
pwndbg> bl
Num     Type           Disp Enb Address            What
1       breakpoint     keep y   0x0000565320689437 in virtio_recv_pkts at ../drivers/net/virtio/virtio_rxtx.c:981
2       breakpoint     keep y   0x000056531e45785f in eth_em_recv_pkts at ../drivers/net/e1000/em_rxtx.c:696
3       breakpoint     keep n   0x000056531d0f59ed in netif_rcv_mbuf at /home/user/dpvs/src/netif.c:2535
	breakpoint already hit 3 times
4       breakpoint     keep y   0x000056531ce98bd9 in __dp_vs_xmit_nat4 at /home/user/dpvs/src/ipvs/ip_vs_xmit.c:1753
5       breakpoint     keep y   0x000056531cd1793b in conn_term_lcore at /home/user/dpvs/src/ipvs/ip_vs_conn.c:1225
6       breakpoint     keep y   0x000056531ce98bd9 in __dp_vs_xmit_nat4 at /home/user/dpvs/src/ipvs/ip_vs_xmit.c:1753
7       breakpoint     keep n   0x000056531ce94b0e in __dp_vs_fast_xmit_fnat4 at /home/user/dpvs/src/ipvs/ip_vs_xmit.c:40
	breakpoint already hit 7 times
8       breakpoint     keep y   0x000056531cd62d2b in tcp_fnat_in_handler at /home/user/dpvs/src/ipvs/ip_vs_proto_tcp.c:898
	breakpoint already hit 36 times
9       breakpoint     keep y   0x000056531cd62d53 /home/user/dpvs/src/ipvs/ip_vs_proto_tcp.c:898
	breakpoint already hit 2 times
10      breakpoint     keep y   0x000056531cd51fc9 in tcp_in_remove_ts at /home/user/dpvs/src/ipvs/ip_vs_proto_tcp.c:279
	breakpoint already hit 3 times


// ACL 
static struct dp_vs_conn *
tcp_conn_lookup(struct dp_vs_proto *proto, const struct dp_vs_iphdr *iph,
                struct rte_mbuf *mbuf, int *direct, bool reverse, bool *drop,
                lcoreid_t *peer_cid)
{
    struct tcphdr *th, _tcph;
    struct dp_vs_conn *conn;
    assert(proto && iph && mbuf);

    th = mbuf_header_pointer(mbuf, iph->len, sizeof(_tcph), &_tcph);
    if (unlikely(!th))
        return NULL;

    if (dp_vs_blklst_filtered(iph->af, iph->proto, &iph->daddr,
                th->dest, &iph->saddr, mbuf)) {
        *drop = true;
        return NULL;
    }

    if (dp_vs_whtlst_filtered(iph->af, iph->proto, &iph->daddr,
                th->dest, &iph->saddr, mbuf)) {
        *drop = true;
        return NULL;
    }
	//...
}
```


### 3. Searching for attack surfaces

```bash

__dp_vs_fast_xmit_fnat4
	netif_xmit
		netif_hard_xmit

	tcp_fnat_in_handler (proto->fnat_in_handler)


# ip/forward/route
ipv4_rcv
	ipv4_rcv_fin
		ipv4_forward
			ipv4_forward_fin
				ipv4_output_fin
					ipv4_output_fin2


# some important files
src/ipvs/ip_vs_proxy_proto.c

src/ipvs/ip_vs_xmit.c

```

### ~~4. Potential ICMP6 OOB~~ 

```c++
int ip6_skip_exthdr(const struct rte_mbuf *imbuf, int start, __u8 *nexthdrp)
{
    __u8 nexthdr = *nexthdrp;

    while (ip6_ext_hdr(nexthdr)) {
        struct ip6_ext _hdr, *hp;
        int hdrlen;

        if (nexthdr == NEXTHDR_NONE)
            return -1;
        hp = mbuf_header_pointer(imbuf, start, sizeof(_hdr), &_hdr);
        if (hp == NULL)
            return -1;
        if (nexthdr == NEXTHDR_FRAGMENT) {
            __be16 _frag_off, *fp;
            fp = mbuf_header_pointer(imbuf,
                        start + offsetof(struct ip6_frag, ip6f_offlg),
                        sizeof(_frag_off),
                        &_frag_off);
            if (fp == NULL)
                return -1;

            if (ntohs(*fp) & ~0x7)
                break;
            hdrlen = 8;
        } else if (nexthdr == NEXTHDR_AUTH)
            hdrlen = (hp->ip6e_len + 2) << 2;
        else
            hdrlen = ((hp)->ip6e_len + 1) << 3;

        nexthdr = hp->ip6e_nxt;
        start += hdrlen;
    }

    *nexthdrp = nexthdr;
    return start;
}
```

Note:

```c++
// stacktrace
pwndbg> bt
#0  __dp_vs_in_icmp4 (mbuf=0x105945dc0, related=0x7f7ba65c952c) at /home/user/dpvs/src/ipvs/ip_vs_core.c:664
#1  0x0000562da6bf80a5 in dp_vs_in_icmp (af=2, mbuf=0x105945dc0, related=0x7f7ba65c952c) at /home/user/dpvs/src/ipvs/ip_vs_core.c:923
#2  0x0000562da6bf819e in __dp_vs_in (priv=0x0, mbuf=0x105945dc0, state=0x7f7ba65c9600, af=2) at /home/user/dpvs/src/ipvs/ip_vs_core.c:957
#3  0x0000562da6bf862f in dp_vs_in (priv=0x0, mbuf=0x105945dc0, state=0x7f7ba65c9600) at /home/user/dpvs/src/ipvs/ip_vs_core.c:1078
#4  0x0000562da6a962d2 in INET_HOOK (af=2, hook=0, mbuf=0x105945dc0, in=0x10022c340, out=0x0, okfn=0x562da6b30c55 <ipv4_rcv_fin>) at /home/user/dpvs/src/inet.c:248
#5  0x0000562da6b31045 in ipv4_rcv (mbuf=0x105945dc0, port=0x10022c340) at /home/user/dpvs/src/ipv4.c:446
#6  0x0000562da6fbfa0b in netif_rcv_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x105945dc0, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2535
#7  0x0000562da6fbb874 in netif_deliver_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x105945dc0, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2453
#8  0x0000562da6fbfe17 in lcore_process_packets (mbufs=0x562dad4c4e98 <lcore_conf+152>, cid=1 '\001', count=1, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2612
#9  0x0000562da6fc3d3c in lcore_job_recv_fwd (arg=0x0) at /home/user/dpvs/src/netif.c:2656
#10 0x0000562da6ffe0e4 in do_lcore_job (job=0x562dace4d680 <netif_jobs>) at /home/user/dpvs/src/scheduler.c:165
#11 0x0000562da6ffe2c6 in dpvs_job_loop (arg=0x0) at /home/user/dpvs/src/scheduler.c:216
#12 0x0000562dac68c727 in eal_thread_loop (arg=0x1) at ../lib/eal/common/eal_common_thread.c:212
#13 0x0000562dac6adb3e in eal_worker_thread_loop (arg=0x1) at ../lib/eal/linux/eal.c:867
#14 0x00007f7ba8c2aea7 in start_thread (arg=<optimized out>) at pthread_create.c:477
#15 0x00007f7ba86d5acf in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:95

```

**=> should set up ipv6!**


```bash
# before binding surfaces
root@syzkaller:/home/user/dpvs/bin# ifconfig
eth0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.0.2.15  netmask 255.255.255.0  broadcast 0.0.0.0
        inet6 2001:db8:10::15  prefixlen 64  scopeid 0x0<global>
        inet6 fe80::5054:ff:fe12:3456  prefixlen 64  scopeid 0x20<link>
        ether 52:54:00:12:34:56  txqueuelen 1000  (Ethernet)
        RX packets 28  bytes 5211 (5.0 KiB)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 38  bytes 6540 (6.3 KiB)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0

lo: flags=73<UP,LOOPBACK,RUNNING>  mtu 65536
        inet 127.0.0.1  netmask 255.0.0.0
        inet6 ::1  prefixlen 128  scopeid 0x10<host>
        loop  txqueuelen 1000  (Local Loopback)
        RX packets 0  bytes 0 (0.0 B)
        RX errors 0  dropped 0  overruns 0  frame 0
        TX packets 0  bytes 0 (0.0 B)
        TX errors 0  dropped 0 overruns 0  carrier 0  collisions 0




```


Fist route:

```c++
pwndbg> bt
#0  dp_vs_fill_iphdr (af=10, mbuf=0x105940140, iph=0x7f240ea74540) at /home/user/dpvs/src/ipvs/ip_vs_core.c:56
#1  0x0000561b523af69a in __dp_vs_pre_routing (priv=0x0, mbuf=0x105940140, state=0x7f240ea74600, af=10) at /home/user/dpvs/src/ipvs/ip_vs_core.c:1093
#2  0x0000561b523af7f4 in dp_vs_pre_routing6 (priv=0x0, mbuf=0x105940140, state=0x7f240ea74600) at /home/user/dpvs/src/ipvs/ip_vs_core.c:1131
#3  0x0000561b5224d2d2 in INET_HOOK (af=10, hook=0, mbuf=0x105940140, in=0x10022c340, out=0x0, okfn=0x561b523221f6 <ip6_rcv_fin>) at /home/user/dpvs/src/inet.c:248
#4  0x0000561b52322621 in ip6_rcv (mbuf=0x105940140, dev=0x10022c340) at /home/user/dpvs/src/ipv6/ipv6.c:698
#5  0x0000561b52776a0b in netif_rcv_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x105940140, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2535
#6  0x0000561b52772874 in netif_deliver_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x105940140, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2453
#7  0x0000561b52776e17 in lcore_process_packets (mbufs=0x561b58c7be98 <lcore_conf+152>, cid=1 '\001', count=1, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2612
#8  0x0000561b5277ad3c in lcore_job_recv_fwd (arg=0x0) at /home/user/dpvs/src/netif.c:2656
#9  0x0000561b527b50e4 in do_lcore_job (job=0x561b58604680 <netif_jobs>) at /home/user/dpvs/src/scheduler.c:165
#10 0x0000561b527b52c6 in dpvs_job_loop (arg=0x0) at /home/user/dpvs/src/scheduler.c:216
#11 0x0000561b57e43727 in eal_thread_loop (arg=0x1) at ../lib/eal/common/eal_common_thread.c:212
#12 0x0000561b57e64b3e in eal_worker_thread_loop (arg=0x1) at ../lib/eal/linux/eal.c:867
#13 0x00007f24110d5ea7 in start_thread (arg=<optimized out>) at pthread_create.c:477
#14 0x00007f2410b80acf in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:95
```


## Step 2: Designing a dpvsfuzzer 

**Entrypoint**

- DPVS registered several backend jobs, such as `lcore_job_recv_fwd`, `lcore_job_xmit`, or `lcore_job_timer_manage`. There is only two interesting functions that attract a lots of attention:
	+ `lcore_job_recv_fwd`: handles packet reception and forwarding for forwarding worker lcors 
	+ `lcore_job_xmit`: manages packet transmission for forwarding worker lcores

```c++
static struct dpvs_lcore_job_array netif_jobs[NETIF_JOB_MAX];
```