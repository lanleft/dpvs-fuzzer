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