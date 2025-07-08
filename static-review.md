# DPVS 

<!-- introduction -->

## Table of Contents

<!-- toc -->

- [Overview](#overview)
- [Code Review](#code-review)
  * [1. `netif.c`](#1-netifc)
  * [2. `ipv4.c`](#2-ipv4c)

<!-- tocstop -->

## Overview

## Code Review

### 1. `netif.c`

```c
static int netif_deliver_mbuf(struct netif_port *dev, lcoreid_t cid,
                  struct rte_mbuf *mbuf, bool pkts_from_ring)
{
    // ...

    /*
     * In NETIF_PORT_FLAG_FORWARD2KNI mode.
     * All packets received are deep copied and sent to KNI
     * for the purpose of capturing forwarding packets.Since the
     * rte_mbuf will be modified in the following procedure,
     * we should use mbuf_copy instead of rte_pktmbuf_clone.
     */
    if (dev->flag & NETIF_PORT_FLAG_FORWARD2KNI) {
        struct rte_mbuf *mbuf_copied = mbuf_copy(mbuf, pktmbuf_pool[dev->socket]); // [1]
        if (likely(mbuf_copied != NULL))
            kni_ingress(mbuf_copied, dev);
        else
            RTE_LOG(WARNING, NETIF, "%s: failed to copy mbuf for kni\n", __func__);
    }
    if (!pkts_from_ring && (dev->flag & NETIF_PORT_FLAG_TC_INGRESS)) { // require NETIF_PORT_FLAG_TC_INGRESS enabled
        mbuf = tc_hook(netif_tc(dev), mbuf, TC_HOOK_INGRESS, &ret);
        if (!mbuf) // how about tc_hook return non-zero value?
            return ret;
    }

    return netif_rcv_mbuf(dev, cid, mbuf, pkts_from_ring);
}

// => that's a legitimate flow, not a potential bug!
// how about race condition?
```

- `netif_rcv_mbuf` is called in `netif_deliver_mbuf`

```c
int netif_rcv_mbuf(struct netif_port *dev, lcoreid_t cid, struct rte_mbuf *mbuf, bool pkts_from_ring)
{
    //...
    forward2kni = (dev->flag & NETIF_PORT_FLAG_FORWARD2KNI) ? true : false;
    pt = pkt_type_get(eth_hdr->ether_type, dev); 
    if (NULL == pt) { // why this place also calls kni_ingress but doesn't create mbuf_copied as [1] does?
        if (!forward2kni) {
            kni_ingress(mbuf, dev);
            goto done;
        }
        goto drop;
    }

    // ...
}


// keep in mind:
static inline char *rte_pktmbuf_adj(struct rte_mbuf *m, uint16_t len)
{
	__rte_mbuf_sanity_check(m, 1);

	if (unlikely(len > m->data_len))
		return NULL;

	/* NB: elaborating the addition like this instead of using
	 *     += allows us to ensure the result type is uint16_t
	 *     avoiding compiler warnings on gcc 8.1 at least */
	m->data_len = (uint16_t)(m->data_len - len);
	m->data_off = (uint16_t)(m->data_off + len);
	m->pkt_len  = (m->pkt_len - len);
	return (char *)m->buf_addr + m->data_off;
}

```

`mbuf` example:

```bash
pwndbg> p mbuf
$1 = (struct rte_mbuf *) 0x105918300
pwndbg> p *mbuf
$2 = {
  buf_addr = 0x105918380,
  buf_iova = 4640048000,
  {
    rearm_data = {4295032960},
    {
      data_off = 128,
      refcnt = 1,
      nb_segs = 1,
      port = 0
    }
  },
  ol_flags = 0,
  {
    rx_descriptor_fields1 = {0x2a00000001, 0x2a, 0x88000000000ffff},
    {
      {
        packet_type = 1,
        {
          l2_type = 1 '\001',
          l3_type = 0 '\000',
          l4_type = 0 '\000',
          tun_type = 0 '\000',
          {
            inner_esp_next_proto = 0 '\000',
            {
              inner_l2_type = 0 '\000',
              inner_l3_type = 0 '\000'
            }
          },
          inner_l4_type = 0 '\000'
        }
      },
      pkt_len = 42,
      data_len = 42,
      vlan_tci = 0,
      {
        hash = {
          rss = 0,
          fdir = {
            {
              {
                hash = 0,
                id = 0
              },
              lo = 0
            },
            hi = 65535
          },
          sched = {
            queue_id = 0,
            traffic_class = 255 '\377',
            color = 255 '\377',
            reserved = 0
          },
          txadapter = {
            reserved1 = 0,
            reserved2 = 65535,
            txq = 0
          },
          usr = 0
        }
      },
      vlan_tci_outer = 0,
      buf_len = 2176
    }
  },
  pool = 0x1040fde80,
  next = 0x0,
  {
    tx_offload = 0,
    {
      l2_len = 0,
      l3_len = 0,
      l4_len = 0,
      tso_segsz = 0,
      outer_l3_len = 0,
      outer_l2_len = 0
    }
  },
  shinfo = 0x0,
  priv_size = 0,
  timesync = 0,
  dynfield1 = {0, 0, 0, 0, 0, 0, 0, 0, 0}
}
pwndbg> 
```

### 2. `ipv4.c`

```c


```

### 3. `inet.c`

```c
int inet_init(void)
{
    int err;

    if ((err = neigh_init()) != 0)
        return err;
    if ((err = route_init()) != 0)
        return err;
    if ((err = route6_init()) != 0)
        return err;
    if ((err = inet_hook_init()) != 0)
        return err;
    if ((err = ipv4_init()) != 0)
        return err;
    if ((err = ipv6_init()) != 0)
        return err;
    if ((err = icmp_init()) != 0)
        return err;
    if ((err = icmpv6_init()) != 0)
        return err;
    if ((err = inet_addr_init()) != 0)
        return err;
    if ((err = dpvs_lldp_init()) != 0)
        return err;

    return EDPVS_OK;
}

```


### 4. Some modules 

```c

/*
 * the initialization order of all the modules
 */
#define DPVS_MODULES {                                          \
        DPVS_MODULE(MODULE_FIRST,       "scheduler",            \
                    dpvs_scheduler_init, dpvs_scheduler_term),  \
        DPVS_MODULE(MODULE_GLOBAL_DATA, "global data",          \
                    global_data_init,    global_data_term),     \
        DPVS_MODULE(MODULE_MBUF,        "mbuf",                 \
                    mbuf_init,           NULL),                 \
        DPVS_MODULE(MODULE_CFG,         "config file",          \
                    cfgfile_init,        cfgfile_term),         \
        DPVS_MODULE(MODULE_PDUMP,        "pdump",               \
                    pdump_init,          pdump_term),           \
        DPVS_MODULE(MODULE_NETIF_VDEV,  "vdevs",                \
                    netif_vdevs_add,     NULL),                 \
        DPVS_MODULE(MODULE_TIMER,       "timer",                \
                    dpvs_timer_init,     dpvs_timer_term),      \
        DPVS_MODULE(MODULE_TC,          "tc",                   \
                    tc_init,             tc_term),              \
        DPVS_MODULE(MODULE_NETIF,       "netif",                \
                    netif_init,          netif_term),           \
        DPVS_MODULE(MODULE_CTRL,        "ctrl",                 \
                    ctrl_init,           ctrl_term),            \
        DPVS_MODULE(MODULE_TC_CTRL,     "tc_ctrl",              \
                    tc_ctrl_init,        tc_ctrl_term),         \
        DPVS_MODULE(MODULE_VLAN,        "vlan",                 \
                    vlan_init,           NULL),                 \
        DPVS_MODULE(MODULE_INET,        "inet",                 \
                    inet_init,           inet_term),            \
        DPVS_MODULE(MODULE_SA_POOL,     "sa_pool",              \
                    sa_pool_init,        sa_pool_term),         \
        DPVS_MODULE(MODULE_IP_TUNNEL,   "tunnel",               \
                    ip_tunnel_init,      ip_tunnel_term),       \
        DPVS_MODULE(MODULE_IPSET,       "ipset",                \
                    ipset_init,          ipset_term),           \
        DPVS_MODULE(MODULE_VS,          "ipvs",                 \
                    dp_vs_init,          dp_vs_term),           \
        DPVS_MODULE(MODULE_NETIF_CTRL,  "netif ctrl",           \
                    netif_ctrl_init,     netif_ctrl_term),      \
        DPVS_MODULE(MODULE_IFTRAF,      "iftraf",               \
                    iftraf_init,         iftraf_term),          \
        DPVS_MODULE(MODULE_EAL_MEM,     "eal_mem",              \
                    eal_mem_init,        eal_mem_term),         \
        DPVS_MODULE(MODULE_LAST,        "last",                 \
                    NULL,                NULL)                  \
    }
```

### 5. `ipv4.c`

- `ipv4_init` registers `ip4_pkt_type` to `netif_register_pkt`

```c

int ipv4_init(void)
{
    int err, i;

    ip4_idents = rte_malloc(NULL, IP4_IDENTS_SZ * sizeof(*ip4_idents),
                            RTE_CACHE_LINE_SIZE);
    if (!ip4_idents)
        return EDPVS_NOMEM;
    ip4_id_hashrnd = (uint32_t)random();
    for (i = 0; i < IP4_IDENTS_SZ; i++)
        rte_atomic32_set(&ip4_idents[i], (uint32_t)random());

    rte_spinlock_init(&inet_prot_lock);
    rte_spinlock_lock(&inet_prot_lock);
    for (i = 0; i < NELEMS(inet_prots); i++)
        inet_prots[i] = NULL;
    rte_spinlock_unlock(&inet_prot_lock);

#ifdef CONFIG_DPVS_IPV4_STATS
    rte_spinlock_init(&ip4_stats_lock);
#endif

    if ((err = ipv4_frag_init()) != EDPVS_OK)
        return err;

    ip4_pkt_type.type = htons(RTE_ETHER_TYPE_IPV4);
    if ((err = netif_register_pkt(&ip4_pkt_type)) != EDPVS_OK) {
        ipv4_frag_term();
        return err;
    }

    return EDPVS_OK;
}

// breakpoint at `ipv4_rcv()`
pwndbg> p *port
$6 = {
  name = "dpdk0\000\000\000\000\000\000\000\000\000\000",
  id = 0,
  type = PORT_TYPE_GENERAL,
  flag = 24851,
  nrxq = 1,
  ntxq = 1,
  rxq_desc_nb = 256,
  txq_desc_nb = 256,
  addr = {
    addr_bytes = "RT\000\022\064V"
  },
  mc = {
    addrs = {
      next = 0x1894f9640,
      prev = 0x1898fd440
    },
    count = 4
  },
  socket = 0,
  hw_header_len = 14,
  mtu = 1500,
  mbuf_pool = 0x1040fde80,
  dev_info = {
    device = 0x55c35f278070,
    driver_name = 0x55c35cb0074a "net_virtio",
    if_index = 0,
    min_mtu = 46,
    max_mtu = 1500,
    dev_flags = 0x1003cf4fc,
    min_rx_bufsize = 64,
    max_rx_bufsize = 4294967295,
    max_rx_pktlen = 9728,
    max_lro_pkt_size = 0,
    max_rx_queues = 1,
    max_tx_queues = 1,
    max_mac_addrs = 64,
    max_hash_mac_addrs = 0,
    max_vfs = 0,
    max_vmdq_pools = 0,
    rx_seg_capa = {
      multi_pools = 0,
      offset_allowed = 0,
      offset_align_log2 = 0,
      max_nseg = 0,
      reserved = 0
    },
    rx_offload_capa = 8733,
    tx_offload_capa = 32801,
    rx_queue_offload_capa = 0,
    tx_queue_offload_capa = 0,
    reta_size = 0,
    hash_key_size = 0 '\000',
    rss_algo_capa = 1,
    flow_type_rss_offloads = 0,
    default_rxconf = {
      rx_thresh = {
        pthresh = 0 '\000',
        hthresh = 0 '\000',
        wthresh = 0 '\000'
      },
      rx_free_thresh = 0,
      rx_drop_en = 0 '\000',
      rx_deferred_start = 0 '\000',
      rx_nseg = 0,
      share_group = 0,
      share_qid = 0,
      offloads = 0,
      rx_seg = 0x0,
      rx_mempools = 0x0,
      rx_nmempool = 0,
      reserved_64s = {0, 0},
      reserved_ptrs = {0x0, 0x0}
    },
    default_txconf = {
      tx_thresh = {
        pthresh = 0 '\000',
        hthresh = 0 '\000',
        wthresh = 0 '\000'
      },
      tx_rs_thresh = 0,
      tx_free_thresh = 0,
      tx_deferred_start = 0 '\000',
      offloads = 0,
      reserved_64s = {0, 0},
      reserved_ptrs = {0x0, 0x0}
    },
    vmdq_queue_base = 0,
    vmdq_queue_num = 0,
    vmdq_pool_base = 0,
    rx_desc_lim = {
      nb_max = 32768,
      nb_min = 32,
      nb_align = 1,
      nb_seg_max = 65535,
      nb_mtu_seg_max = 65535
    },
    tx_desc_lim = {
      nb_max = 32768,
      nb_min = 32,
      nb_align = 1,
      nb_seg_max = 65535,
      nb_mtu_seg_max = 65535
    },
    speed_capa = 0,
    nb_rx_queues = 0,
    nb_tx_queues = 0,
    max_rx_mempools = 0,
    default_rxportconf = {
      burst_size = 0,
      ring_size = 0,
      nb_queues = 0
    },
    default_txportconf = {
      burst_size = 0,
      ring_size = 0,
      nb_queues = 0
    },
    dev_capa = 0,
    switch_info = {
      name = 0x0,
      domain_id = 65535,
      port_id = 0,
      rx_domain = 0
    },
    err_handle_mode = RTE_ETH_ERROR_HANDLE_MODE_NONE,
    reserved_64s = {0, 0},
    reserved_ptrs = {0x0, 0x0}
  },
  dev_conf = {
    link_speeds = 0,
    rxmode = {
      mq_mode = RTE_ETH_MQ_RX_NONE,
      mtu = 1500,
      max_lro_pkt_size = 0,
      offloads = 1,
      reserved_64s = {0, 0},
      reserved_ptrs = {0x0, 0x0}
    },
    txmode = {
      mq_mode = RTE_ETH_MQ_TX_NONE,
      offloads = 0,
      pvid = 0,
      hw_vlan_reject_tagged = 0 '\000',
      hw_vlan_reject_untagged = 0 '\000',
      hw_vlan_insert_pvid = 0 '\000',
      reserved_64s = {0, 0},
      reserved_ptrs = {0x0, 0x0}
    },
    lpbk_mode = 0,
    rx_adv_conf = {
      rss_conf = {
        rss_key = 0x0,
        rss_key_len = 0 '\000',
        rss_hf = 0,
        algorithm = RTE_ETH_HASH_FUNCTION_DEFAULT
      },
      vmdq_dcb_conf = {
        nb_queue_pools = 0,
        enable_default_pool = 0 '\000',
        default_pool = 0 '\000',
        nb_pool_maps = 0 '\000',
        pool_map = {{
            vlan_id = 0,
            pools = 0
          } <repeats 64 times>},
        dcb_tc = "\000\000\000\000\000\000\000"
      },
      dcb_rx_conf = {
        nb_tcs = 0,
        dcb_tc = "\000\000\000\000\000\000\000"
      },
      vmdq_rx_conf = {
        nb_queue_pools = 0,
        enable_default_pool = 0 '\000',
        default_pool = 0 '\000',
        enable_loop_back = 0 '\000',
        nb_pool_maps = 0 '\000',
        rx_mode = 0,
        pool_map = {{
            vlan_id = 0,
            pools = 0
          } <repeats 64 times>}
      }
    },
    tx_adv_conf = {
      vmdq_dcb_tx_conf = {
        nb_queue_pools = 0,
        dcb_tc = "\000\000\000\000\000\000\000"
      },
      dcb_tx_conf = {
        nb_tcs = 0,
        dcb_tc = "\000\000\000\000\000\000\000"
      },
      vmdq_tx_conf = {
        nb_queue_pools = 0
      }
    },
    dcb_capability_en = 0,
    intr_conf = {
      lsc = 0,
      rxq = 0,
      rmv = 0
    }
  },
  stats = {
    ipackets = 0,
    opackets = 0,
    ibytes = 0,
    obytes = 0,
    imissed = 0,
    ierrors = 0,
    oerrors = 0,
    rx_nombuf = 0,
    q_ipackets = {0 <repeats 16 times>},
    q_opackets = {0 <repeats 16 times>},
    q_ibytes = {0 <repeats 16 times>},
    q_obytes = {0 <repeats 16 times>},
    q_errors = {0 <repeats 16 times>}
  },
  dev_lock = {
    cnt = 0
  },
  list = {
    next = 0x55c35d5a7ba0 <port_tab>,
    prev = 0x55c35d5a7ba0 <port_tab>
  },
  nlist = {
    next = 0x55c35d5a8d60 <port_ntab+448>,
    prev = 0x55c35d5a8d60 <port_ntab+448>
  },
  in_ptr = 0x1003c47c0,
  kni = {
    name = '\000' <repeats 15 times>,
    kni = 0x0,
    addr = {
      addr_bytes = "\000\000\000\000\000"
    },
    kni_rtnl_timer = {
      list = {
        next = 0x0,
        prev = 0x0
      },
      handler = 0x0,
      priv = 0x0,
      is_period = false,
      delay = 0,
      left = 0
    },
    kni_rtnl_fd = 0,
    flags = 0 '\000',
    rx_ring = 0x0,
    kni_flows = {
      next = 0x0,
      prev = 0x0
    }
  },
  bond = 0x10022d280,
  vlan_info = 0x0,
  tc = {{
      dev = 0x10022c340,
      tc_mbuf_pool = 0x104afdec0,
      qsch_cnt = 0,
      qsch = 0x0,
      qsch_hash = 0x1003c4540,
      qsch_hash_size = 64,
      qsch_ingress = 0x0
    }, {
      dev = 0x10022c340,
      tc_mbuf_pool = 0x104afdec0,
      qsch_cnt = 0,
      qsch = 0x0,
      qsch_hash = 0x1003c42c0,
      qsch_hash_size = 64,
      qsch_ingress = 0x0
    }, {
      dev = 0x10022c340,
      tc_mbuf_pool = 0x104afdec0,
      qsch_cnt = 0,
      qsch = 0x0,
      qsch_hash = 0x1003c9440,
      qsch_hash_size = 64,
      qsch_ingress = 0x0
    }, {
      dev = 0x10022c340,
      tc_mbuf_pool = 0x104afdec0,
      qsch_cnt = 0,
      qsch = 0x0,
      qsch_hash = 0x1003c91c0,
      qsch_hash_size = 64,
      qsch_ingress = 0x0
    }},
  netif_ops = 0x55c35cf33b00 <dpdk_netif_ops>
}

```


### 5. Packet type function

```c++
// in netif_rcv_mbuf
    err = pt->func(mbuf, dev);

// func can be 
pwndbg> p *pt
$4 = {
  type = 1544,
  port = 0x0,
  func = 0x5555557ab3b0 <neigh_resolve_input>,
  list = {
    next = 0x555555eae798 <ip4_pkt_type+24>,
    prev = 0x5555568e1320 <pkt_type_tab+128>
  }
}
// ========================
pwndbg> p *pt
$5 = {
  type = 8,
  port = 0x0,
  func = 0x5555557ce810 <ipv4_rcv>,
  list = {
    next = 0x5555568e1320 <pkt_type_tab+128>,
    prev = 0x555555eae4d8 <arp_pkt_type+24>
  }
}

```


### 6. Some more functions need to cover

```c++

static int tcp_in_prune_options(int af, int reqlen, struct rte_mbuf *mbuf, struct tcphdr *tcph)
{
    // ...
}



```