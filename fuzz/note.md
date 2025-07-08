

## Note 

### need to modify 


```c++
void install_netif_keywords(void)
{
    install_keyword_root("netif_defs", netif_defs_handler);
    install_keyword("pktpool_size", pktpool_size_handler, KW_TYPE_INIT);
    install_keyword("pktpool_cache", pktpool_cache_handler, KW_TYPE_INIT);
    install_keyword("fdir_mode", fdir_mode_handler, KW_TYPE_INIT);
    install_keyword("device", device_handler, KW_TYPE_INIT);
    install_sublevel();
    install_keyword("rx", NULL, KW_TYPE_INIT);
    install_sublevel();
    install_keyword("queue_number", rx_queue_number_handler, KW_TYPE_INIT);
    install_keyword("descriptor_number", rx_desc_nb_handler, KW_TYPE_INIT);
    install_keyword("rss", rss_handler, KW_TYPE_INIT);
    install_sublevel_end();
    install_keyword("tx", NULL, KW_TYPE_INIT);
    install_sublevel();
    install_keyword("queue_number", tx_queue_number_handler, KW_TYPE_INIT);
    install_keyword("descriptor_number", tx_desc_nb_handler, KW_TYPE_INIT);
    install_keyword("mbuf_fast_free", tx_mbuf_fast_free_handler, KW_TYPE_INIT);
    install_sublevel_end();
    install_keyword("promisc_mode", promisc_mode_handler, KW_TYPE_INIT);
    install_keyword("allmulticast", allmulticast_handler, KW_TYPE_INIT);
    install_keyword("mtu", custom_mtu_handler,KW_TYPE_INIT);
    install_keyword("kni_name", kni_name_handler, KW_TYPE_INIT);
    install_sublevel_end();
    install_keyword("bonding", bonding_handler, KW_TYPE_INIT);
    install_sublevel();
    install_keyword("mode", bonding_mode_handler, KW_TYPE_INIT);
    install_keyword("slave", bonding_slave_handler, KW_TYPE_INIT);
    install_keyword("primary", bonding_primary_handler, KW_TYPE_INIT);
    install_keyword("numa_node", bonding_numa_node_handler, KW_TYPE_INIT);
    install_keyword("kni_name", bonding_kni_name_handler, KW_TYPE_INIT);
    install_keyword("options", bonding_options_handler, KW_TYPE_INIT);
    install_sublevel_end();

    install_keyword_root("worker_defs", worker_defs_handler);
    install_keyword("worker", worker_handler, KW_TYPE_INIT);
    install_sublevel();
    install_keyword("type", worker_type_handler, KW_TYPE_INIT);
    install_keyword("cpu_id", cpu_id_handler, KW_TYPE_INIT);
#ifdef CONFIG_ICMP_REDIRECT_CORE
    install_keyword("icmp_redirect_core", cpu_icmp_redirect_handler, KW_TYPE_INIT);
#endif
    install_keyword("port", worker_port_handler, KW_TYPE_INIT);
    install_sublevel();
    install_keyword("rx_queue_ids", rx_queue_ids_handler, KW_TYPE_INIT);
    install_keyword("tx_queue_ids", tx_queue_ids_handler, KW_TYPE_INIT);
    install_keyword("isol_rx_cpu_ids", isol_rx_cpu_ids_handler, KW_TYPE_INIT);
    install_keyword("isol_rxq_ring_sz", isol_rxq_ring_sz_handler, KW_TYPE_INIT);
    install_sublevel_end();
    install_sublevel_end();
}
```


### Should make testcase go through

```bash
tcp_out_adjust_seq
tcp_out_adjust_mss
tcp_fnat_out_handler # ?? setting up fnat
dp_vs_proto_tcp

ip6_pkt_type


# neigh
neigh_hash
neigh_lookup_entry
neigh_output
...
neigh_resolve_input # missing neigh resolve packet


# arp packet
RTE_ETHER_TYPE_ARP
arp_opcode


# ipvs core
dp_vs_snat_schedule
dp_vs_schedule
__dp_vs_in_icmp4
__dp_vs_in
INET_HOOK
ipv4_rcv
ipv4_rcv_fin


```





## Code

```bash

arp_pkt_type -> neigh_resolve_input
ipv4_rcv

```


## Enum

```c++

/* Standard well-defined IP protocols.  */
enum
  {
    IPPROTO_IP = 0,	   /* Dummy protocol for TCP.  */
#define IPPROTO_IP		IPPROTO_IP
    IPPROTO_ICMP = 1,	   /* Internet Control Message Protocol.  */
#define IPPROTO_ICMP		IPPROTO_ICMP
    IPPROTO_IGMP = 2,	   /* Internet Group Management Protocol. */
#define IPPROTO_IGMP		IPPROTO_IGMP
    IPPROTO_IPIP = 4,	   /* IPIP tunnels (older KA9Q tunnels use 94).  */
#define IPPROTO_IPIP		IPPROTO_IPIP
    IPPROTO_TCP = 6,	   /* Transmission Control Protocol.  */
#define IPPROTO_TCP		IPPROTO_TCP
    IPPROTO_EGP = 8,	   /* Exterior Gateway Protocol.  */
#define IPPROTO_EGP		IPPROTO_EGP
    IPPROTO_PUP = 12,	   /* PUP protocol.  */
#define IPPROTO_PUP		IPPROTO_PUP
    IPPROTO_UDP = 17,	   /* User Datagram Protocol.  */
#define IPPROTO_UDP		IPPROTO_UDP
    IPPROTO_IDP = 22,	   /* XNS IDP protocol.  */
#define IPPROTO_IDP		IPPROTO_IDP
    IPPROTO_TP = 29,	   /* SO Transport Protocol Class 4.  */
#define IPPROTO_TP		IPPROTO_TP
    IPPROTO_DCCP = 33,	   /* Datagram Congestion Control Protocol.  */
#define IPPROTO_DCCP		IPPROTO_DCCP
    IPPROTO_IPV6 = 41,     /* IPv6 header.  */
#define IPPROTO_IPV6		IPPROTO_IPV6
    IPPROTO_RSVP = 46,	   /* Reservation Protocol.  */
#define IPPROTO_RSVP		IPPROTO_RSVP
    IPPROTO_GRE = 47,	   /* General Routing Encapsulation.  */
#define IPPROTO_GRE		IPPROTO_GRE
    IPPROTO_ESP = 50,      /* encapsulating security payload.  */
#define IPPROTO_ESP		IPPROTO_ESP
    IPPROTO_AH = 51,       /* authentication header.  */
#define IPPROTO_AH		IPPROTO_AH
    IPPROTO_MTP = 92,	   /* Multicast Transport Protocol.  */
#define IPPROTO_MTP		IPPROTO_MTP
    IPPROTO_BEETPH = 94,   /* IP option pseudo header for BEET.  */
#define IPPROTO_BEETPH		IPPROTO_BEETPH
    IPPROTO_ENCAP = 98,	   /* Encapsulation Header.  */
#define IPPROTO_ENCAP		IPPROTO_ENCAP
    IPPROTO_PIM = 103,	   /* Protocol Independent Multicast.  */
#define IPPROTO_PIM		IPPROTO_PIM
    IPPROTO_COMP = 108,	   /* Compression Header Protocol.  */
#define IPPROTO_COMP		IPPROTO_COMP
    IPPROTO_SCTP = 132,	   /* Stream Control Transmission Protocol.  */
#define IPPROTO_SCTP		IPPROTO_SCTP
    IPPROTO_UDPLITE = 136, /* UDP-Lite protocol.  */
#define IPPROTO_UDPLITE		IPPROTO_UDPLITE
    IPPROTO_MPLS = 137,    /* MPLS in IP.  */
#define IPPROTO_MPLS		IPPROTO_MPLS
    IPPROTO_ETHERNET = 143, /* Ethernet-within-IPv6 Encapsulation.  */
#define IPPROTO_ETHERNET	IPPROTO_ETHERNET
    IPPROTO_RAW = 255,	   /* Raw IP packets.  */
#define IPPROTO_RAW		IPPROTO_RAW
    IPPROTO_MPTCP = 262,   /* Multipath TCP connection.  */
#define IPPROTO_MPTCP		IPPROTO_MPTCP
    IPPROTO_MAX
  };

/* If __USE_KERNEL_IPV6_DEFS is 1 then the user has included the kernel
   network headers first and we should use those ABI-identical definitions
   instead of our own, otherwise 0.  */
#if !__USE_KERNEL_IPV6_DEFS
enum
  {
    IPPROTO_HOPOPTS = 0,   /* IPv6 Hop-by-Hop options.  */
#define IPPROTO_HOPOPTS		IPPROTO_HOPOPTS
    IPPROTO_ROUTING = 43,  /* IPv6 routing header.  */
#define IPPROTO_ROUTING		IPPROTO_ROUTING
    IPPROTO_FRAGMENT = 44, /* IPv6 fragmentation header.  */
#define IPPROTO_FRAGMENT	IPPROTO_FRAGMENT
    IPPROTO_ICMPV6 = 58,   /* ICMPv6.  */
#define IPPROTO_ICMPV6		IPPROTO_ICMPV6
    IPPROTO_NONE = 59,     /* IPv6 no next header.  */
#define IPPROTO_NONE		IPPROTO_NONE
    IPPROTO_DSTOPTS = 60,  /* IPv6 destination options.  */
#define IPPROTO_DSTOPTS		IPPROTO_DSTOPTS
    IPPROTO_MH = 135       /* IPv6 mobility header.  */
#define IPPROTO_MH		IPPROTO_MH
  };
```


## Forwarding

```bash

dpvs_add_service


```


## Routing

```bash
ifa_add_route4
ifa_add_route
ifa_sockopt_set

```


## Packet type

### Type 

```bash
pkt_type_get
    neigh_resolve_input
    ipv4_rcv
    
# in real target
pkt_type_get
    neigh_resolve_inputc
    ipv4_rcv


```

### Inet hook

```c++
/*
 * Inet Hooks
 */
enum {
    INET_HOOK_PRE_ROUTING,
    INET_HOOK_LOCAL_IN,
    INET_HOOK_FORWARD,
    INET_HOOK_LOCAL_OUT,
    INET_HOOK_POST_ROUTING,
    INET_HOOK_NUMHOOKS,
};

```

## Understand connection state

```bash
dp_vs_conn_free

```



## Type bugs

### Race condition

```c++
rte_spinlock_lock(&inet_prot_lock);
prot = inet_prots[iph->next_proto_id]; // reading here
if (prot)
    handler = prot->handler;
rte_spinlock_unlock(&inet_prot_lock);

// be changed or freed by thread B
// ... any operation later is invalid 
```

- This lock prevents data race condition, without the lock:
    + thread A reads from `inet_prots[iph->next_proto_id]` while 
    + thread B is modifying it
=> This could lead to reading partially updated data or even crashes (null pointer dereference, UAF)

Potential variants:

```c++
static int icmp_rcv(struct rte_mbuf *mbuf)
{
    struct rte_ipv4_hdr *iph = MBUF_USERDATA(mbuf, struct rte_ipv4_hdr *, MBUF_FIELD_PROTO);
    struct rte_icmp_hdr *ich;
    struct icmp_ctrl *ctrl;

    if (mbuf_may_pull(mbuf, sizeof(struct rte_icmp_hdr)) != 0)
        goto invpkt;
    ich = rte_pktmbuf_mtod(mbuf, struct rte_icmp_hdr *);

    if (unlikely(!iph)) {
        RTE_LOG(WARNING, ICMP, "%s: no ipv4 header\n", __func__);
        goto invpkt;
    }

#ifdef CONFIG_DPVS_ICMP_DEBUG
    icmp_dump_hdr(mbuf);
#endif

    ctrl = &icmp_ctrls[ich->icmp_type]; // doesn't have lock here
    if (ctrl->handler)
        return ctrl->handler(mbuf);
    else
        return EDPVS_KNICONTINUE; /* KNI may like it, don't drop */

invpkt:
    rte_pktmbuf_free(mbuf);
    return EDPVS_INVPKT;
}

```

### Use-after-free

- Focus on connection management

```bash
dp_vs_conn_put -> decrease refcnt
dp_vs_conn_free -> free conn

# timeout handler
dp_vs_conn_expire
dp_vs_conn_attach_timer
```

- Connection:

```c++
pwndbg> bt
#0  dp_vs_conn_new (mbuf=0x105906440, iph=0x7fd0c51c4540, param=0x7fd0c51c4360, dest=0x1898fe440, flags=0) at /home/user/dpvs/src/ipvs/ip_vs_conn.c:815
#1  0x0000560ff6dd57c8 in dp_vs_schedule (svc=0x1898fe080, iph=0x7fd0c51c4540, mbuf=0x105906440, is_synproxy_on=false) at /home/user/dpvs/src/ipvs/ip_vs_core.c:349
#2  0x0000560ff6e0b938 in tcp_conn_sched (proto=0x560ffd028840 <dp_vs_proto_tcp>, iph=0x7fd0c51c4540, mbuf=0x105906440, conn=0x7fd0c51c4538, verdict=0x7fd0c51c4530) at /home/user/dpvs/src/ipvs/ip_vs_proto_tcp.c:812
#3  0x0000560ff6dd7331 in __dp_vs_in (priv=0x0, mbuf=0x105906440, state=0x7fd0c51c4600, af=2) at /home/user/dpvs/src/ipvs/ip_vs_core.c:1005
#4  0x0000560ff6dd762f in dp_vs_in (priv=0x0, mbuf=0x105906440, state=0x7fd0c51c4600) at /home/user/dpvs/src/ipvs/ip_vs_core.c:1078
#5  0x0000560ff6c752d2 in INET_HOOK (af=2, hook=0, mbuf=0x105906440, in=0x10022c340, out=0x0, okfn=0x560ff6d0fc55 <ipv4_rcv_fin>) at /home/user/dpvs/src/inet.c:248
#6  0x0000560ff6d10045 in ipv4_rcv (mbuf=0x105906440, port=0x10022c340) at /home/user/dpvs/src/ipv4.c:446
#7  0x0000560ff719ea0b in netif_rcv_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x105906440, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2535
#8  0x0000560ff719a874 in netif_deliver_mbuf (dev=0x10022c340, cid=1 '\001', mbuf=0x105906440, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2453
#9  0x0000560ff719ee17 in lcore_process_packets (mbufs=0x560ffd6a3e98 <lcore_conf+152>, cid=1 '\001', count=1, pkts_from_ring=false) at /home/user/dpvs/src/netif.c:2612
#10 0x0000560ff71a2d3c in lcore_job_recv_fwd (arg=0x0) at /home/user/dpvs/src/netif.c:2656
#11 0x0000560ff71dd0e4 in do_lcore_job (job=0x560ffd02c680 <netif_jobs>) at /home/user/dpvs/src/scheduler.c:165
#12 0x0000560ff71dd2c6 in dpvs_job_loop (arg=0x0) at /home/user/dpvs/src/scheduler.c:216
#13 0x0000560ffc86b727 in eal_thread_loop (arg=0x1) at ../lib/eal/common/eal_common_thread.c:212
#14 0x0000560ffc88cb3e in eal_worker_thread_loop (arg=0x1) at ../lib/eal/linux/eal.c:867
#15 0x00007fd0c7825ea7 in start_thread (arg=<optimized out>) at pthread_create.c:477
#16 0x00007fd0c72d0acf in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:95


// protocol handler

int dp_vs_proto_init(void)
{
    int err;

    if ((err = proto_register(&dp_vs_proto_udp)) != EDPVS_OK) {
        RTE_LOG(ERR, IPVS, "%s: fail to register UDP\n", __func__);
        return err;
    }

    if ((err = proto_register(&dp_vs_proto_tcp)) != EDPVS_OK) {
        RTE_LOG(ERR, IPVS, "%s: fail to register TCP\n", __func__);
        goto tcp_error;
    }

    if ((err = proto_register(&dp_vs_proto_sctp)) != EDPVS_OK) {
        RTE_LOG(ERR, IPVS, "%s: fail to register SCTP\n", __func__);
        goto sctp_error;
    }

    if ((err = proto_register(&dp_vs_proto_icmp6)) != EDPVS_OK) {
        RTE_LOG(ERR, IPVS, "%s: fail to register ICMPV6\n", __func__);
        goto icmp6_error;
    }

    if ((err = proto_register(&dp_vs_proto_icmp)) != EDPVS_OK) {
        RTE_LOG(ERR, IPVS, "%s: fail to register ICMP\n", __func__);
        goto icmp_error;
    }

    return EDPVS_OK;
}

/* timeout hanlder */
static int dp_vs_conn_expire(void *priv)
{}

/* global connection template table */
static struct list_head *dp_vs_ct_tbl;

// what if adding connection step does not have lock?
```
