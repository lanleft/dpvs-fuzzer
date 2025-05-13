#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "sys_time.h"

#if __cplusplus
extern "C" {
#endif

#include "dpvs-config.h"
#include "netif.h"
#include "scheduler.h"
#include "rte_malloc.h"

// include from dpvs main.c 
#include "pidfile.h"
#include "dpdk.h"
#include "conf/common.h"
#include "log.h"
#include "vlan.h"
#include "inet.h"
#include "timer.h"
#include "ctrl.h"
#include "sa_pool.h"
#include "cfgfile.h"
#include "ip_tunnel.h"
#include "sys_time.h"
#include "route6.h"
#include "iftraf.h"
#include "eal_mem.h"
#include "pdump.h"
#include "rte_eal.h"

// include for DPDK

#if __cplusplus
}
#endif

#include "src/libfuzzer/libfuzzer_macro.h"
#include "mbuf_mutator.pb.h"

// Global variables
static struct rte_mempool *mbuf_pool = NULL;
static struct netif_port *test_port = NULL;

// Initialize DPDK environment and mbuf pool
static int init_dpdk(void) {
    int ret;
    char pool_name[32];
    char *argv[10];

    argv[0] = (char *)malloc(10);
    memset(argv[0], 0, 10);
    strcpy(argv[0], "dpvs\0");

    printf("Initializing DPDK environment\n");
    // Initialize EAL

    ret = rte_eal_init(1, argv);
    if (ret < 0) {
        return -1;
    }
    
    // Create mbuf pool
    snprintf(pool_name, sizeof(pool_name), "mbuf_pool_%d", rte_socket_id());
    mbuf_pool = rte_pktmbuf_pool_create(pool_name, 1024, 0, 0,
                                       RTE_MBUF_DEFAULT_BUF_SIZE,
                                       rte_socket_id());
    if (mbuf_pool == NULL) {
        return -1;
    }
    
    // Create a test network interface
    test_port = netif_alloc(NETIF_PORT_ID_INVALID, 0, "test%d", 1, 1, NULL);
    if (!test_port) {
        return -1;
    }
    
    // Set up basic port configuration
    test_port->mbuf_pool = mbuf_pool;
    test_port->flag |= NETIF_PORT_FLAG_ENABLED;
    test_port->mtu = 1500;
    
    return 0;
}

// Convert protobuf message to rte_mbuf
static struct rte_mbuf* create_mbuf_from_proto(const dpvs::MbufMutator& proto) {
    struct rte_mbuf *mbuf;
    uint8_t *data;
    
    // Allocate mbuf
    mbuf = rte_pktmbuf_alloc(mbuf_pool);
    if (!mbuf) {
        return NULL;
    }
    
    // Reset mbuf
    mbuf_userdata_reset(mbuf);
    
    // Set basic fields
    mbuf->pkt_len = proto.pkt_len();
    mbuf->data_len = proto.data_len();
    mbuf->port = proto.port();
    mbuf->nb_segs = proto.nb_segs();
    mbuf->ol_flags = proto.ol_flags();
    
    // Copy headers and payload
    data = rte_pktmbuf_mtod(mbuf, uint8_t*);
    
    // Copy ethernet header
    if (proto.has_eth_header()) {
        memcpy(data, proto.eth_header().data(), proto.eth_header().size());
        data += proto.eth_header().size();
    }
    
    // Set VLAN tag if present
    if (proto.has_vlan_tci()) {
        mbuf->vlan_tci = proto.vlan_tci();
        mbuf->ol_flags |= RTE_MBUF_F_RX_VLAN_STRIPPED;
    }
    
    // Copy IP header if present
    if (proto.has_ip_header()) {
        memcpy(data, proto.ip_header().data(), proto.ip_header().size());
        data += proto.ip_header().size();
    }
    
    // Copy transport header if present
    if (proto.has_transport_header()) {
        memcpy(data, proto.transport_header().data(), proto.transport_header().size());
        data += proto.transport_header().size();
    }
    
    // Copy payload if present
    if (proto.has_payload()) {
        memcpy(data, proto.payload().data(), proto.payload().size());
    }
    
    return mbuf;
}

// Main fuzzer function
DEFINE_PROTO_FUZZER(const dpvs::MbufMutator& proto) {
    static bool initialized = false;
    struct rte_mbuf *mbuf;
    
    printf("Begining fuzzing\n");
    if (!initialized) {
        if (init_dpdk() < 0) {
            return;
        }
        initialized = true;
    }
    
    printf("Creating mbuf from protobuf message\n");
    // Create mbuf from protobuf message
    mbuf = create_mbuf_from_proto(proto);
    if (!mbuf) {
        return;
    }
    
    printf("Calling netif_rcv_mbuf\n");
    // Call the target function
    netif_rcv_mbuf(test_port, rte_lcore_id(), mbuf, false);
    
    printf("Freeing mbuf\n");
    // Cleanup
    rte_pktmbuf_free(mbuf);
}


