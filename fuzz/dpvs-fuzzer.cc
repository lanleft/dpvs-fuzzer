#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#include "rte_ether.h"
#include "rte_ip4.h"
#include "rte_ip6.h"
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <netinet/tcp.h>
#include <rte_tcp.h>
#include <netinet/ip6.h>
#include <arpa/inet.h> 

#if __cplusplus
extern "C" {
#endif

#include "ipvs/laddr.h"
#include "ipvs/dest.h"
#include "ipvs/service.h"
#include "route.h"
#include "dpip/utils.h"
#include "conf/route.h"
#include "sockopt.h"
#include "conf/inetaddr.h"
#include "dpvs-config.h"
#include "netif.h"
#include "scheduler.h"
#include "global_data.h"
#include "rte_malloc.h"

#include "ipv4.h"
#include "neigh.h"
#include "ipset/ipset.h"
#include "ipvs/ipvs.h"
#include "linux_ipv6.h"

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
#include "rte_log.h"
#include "rte_ip6.h"

// include for DPDK

#if __cplusplus
}
#endif

#include "src/libfuzzer/libfuzzer_macro.h"
#include "src/text_format.h"
#include "mbuf_mutator.pb.h"


#define DPVS    "dpvs"
#define RTE_LOGTYPE_DPVS RTE_LOGTYPE_USER1
#define LCORE_CONF_BUFFER_LEN 4096
#define ETH_TYPE_IPV4 0x0800
#define ETH_TYPE_IPV6 0x86dd
#define ETH_TYPE_ARP 0x0806

// Global variables
// static struct rte_mempool *mbuf_pool = pktmbuf_pool[0];
static struct netif_port *dev_port = NULL;
static bool initialized = false;

// route
static int add_route(void);
static int add_dpvs_virtual_service(void);

// packet 
void fill_eth_hdr(struct rte_ether_hdr *eth);
void fill_eth6_hdr(struct rte_ether_hdr *eth);
void fill_in6_addr(struct rte_ipv6_addr *sai, enum dpvs::In6Addr addr);
void fill_ip4_hdr(const dpvs::Ip4Hdr& ip4_hdr_proto, struct rte_ipv4_hdr *ip4_hdr, int proto);
void fill_ip6_hdr(const dpvs::Ip6Hdr& ip6_hdr_proto, struct ip6_hdr *ip6_hdr);
void fill_ip4_src_addr(rte_be32_t *sai, enum dpvs::SrcAddr addr);
void fill_ip4_dst_addr(rte_be32_t *sai, enum dpvs::DstAddr addr);
struct rte_mbuf *DoTcp6Packet(const dpvs::Tcp6Packet& tcp6_packet, struct rte_mbuf *mbuf);
struct rte_mbuf * DoIp6Packet(const dpvs::Ip6Packet& ip6_packet, struct rte_mbuf *mbuf);
struct rte_mbuf * DoIp4Packet(const dpvs::Ip4Packet& ip4_packet, struct rte_mbuf *mbuf);


// struct ip6_hdrctl
//   {
//     uint32_t ip6_un1_flow;   /* 4 bits version, 8 bits TC,
//         20 bits flow-ID */
//     uint16_t ip6_un1_plen;   /* payload length */
//     uint8_t  ip6_un1_nxt;    /* next header */
//     uint8_t  ip6_un1_hlim;   /* hop limit */
// };

// struct dpvs_ip6_hdr
//   {
//     union
//       {
//         struct ip6_hdrctl ip6_un1;
//         uint8_t ip6_un2_vfc;       /* 4 bits version, top 4 bits tclass */
//       } ip6_ctlun;
//     struct in6_addr ip6_src;      /* source address */
//     struct in6_addr ip6_dst;      /* destination address */
//   };


static bool modules_init(void) {

    // scheduler_init
    printf("[INIT_MODULES] Calling scheduler_init\n");
    if (dpvs_scheduler_init() < 0) {
        return false;
    }

    // global_data_init
    printf("[INIT_MODULES] Calling global_data_init\n");
    if (global_data_init() < 0) {
        return false;
    }

    // mbuf_init
    printf("[INIT_MODULES] Calling mbuf_init\n");
    if (mbuf_init() < 0) {
        return false;
    }

    // cfgfile_init
    printf("[INIT_MODULES] Calling cfgfile_init\n");
    if (cfgfile_init() < 0) {
        return false;
    }

    // pdump_init
    printf("[INIT_MODULES] Calling pdump_init\n");
    if (pdump_init() < 0) {
        return false;
    }

    // netif_vdevs_add
    printf("[INIT_MODULES] Calling netif_vdevs_add\n");
    if (netif_vdevs_add() < 0) {
        return false;
    }

    // dpvs_timer_init
    printf("[INIT_MODULES] Calling dpvs_timer_init\n");
    if (dpvs_timer_init() < 0) {
        return false;
    }

    // tc_init
    printf("[INIT_MODULES] Calling tc_init\n");
    if (tc_init() < 0) {
        return false;
    }

    // Setup netif_init
    printf("[INIT_MODULES] Calling netif_init\n");
    if (netif_init() < 0) {
        return false;
    }

    // init ctrl_init module
    printf("[INIT_MODULES] Calling ctrl_init\n");
    if (ctrl_init() < 0) {
        return false;
    }

    // tc_ctrl_init
    printf("[INIT_MODULES] Calling tc_ctrl_init\n");
    if (tc_ctrl_init() < 0) {
        return false;
    }

    // init vlan module
    printf("[INIT_MODULES] Calling vlan_init\n");
    if (vlan_init() < 0) {
        return false;
    }

    // init inet module inet_init
    printf("[INIT_MODULES] Calling inet_init\n");
    if (inet_init() < 0) {
        return false;
    }

    // sa_pool_init
    printf("[INIT_MODULES] Calling sa_pool_init\n");
    if (sa_pool_init() < 0) {
        return false;
    }

    // ip_tunnel_init
    printf("[INIT_MODULES] Calling ip_tunnel_init\n");
    if (ip_tunnel_init() < 0) {
        return false;
    }

    //  ipset_init
    printf("[INIT_MODULES] Calling ipset_init\n");
    if (ipset_init() < 0) {
        return false;
    }

    //  dp_vs_init
    printf("[INIT_MODULES] Calling dp_vs_init\n");
    if (dp_vs_init() < 0) {
        return false;
    }

    //  netif_ctrl_init
    printf("[INIT_MODULES] Calling netif_ctrl_init\n");
    if (netif_ctrl_init() < 0) {
        return false;
    }

    //  iftraf_init
    printf("[INIT_MODULES] Calling iftraf_init\n");
    if (iftraf_init() < 0) {
        return false;
    }

    //  eal_mem_init
    printf("[INIT_MODULES] Calling eal_mem_init\n");
    if (eal_mem_init() < 0) {
        return false;
    }

    return true;
}


static int set_all_thread_affinity(void)
{
    int s;
    lcoreid_t cid;
    pthread_t tid;
    cpu_set_t cpuset;
    unsigned long long cpumask=0;

    tid = pthread_self();
    CPU_ZERO(&cpuset);
    for (cid = 0; cid < RTE_MAX_LCORE; cid++)
        CPU_SET(cid, &cpuset);

    s = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
    if (s != 0) {
        errno = s;
        perror("fail to set thread affinty");
        return -1;
    }

    CPU_ZERO(&cpuset);
    s = pthread_getaffinity_np(tid, sizeof(cpu_set_t), &cpuset);
    if (s != 0) {
        errno = s;
        perror("fail to get thread affinity");
        return -2;
    }

    for (cid = 0; cid < RTE_MAX_LCORE; cid++) {
        if (CPU_ISSET(cid, &cpuset))
            cpumask |= (1LL << cid);
    }
    printf("current thread affinity is set to %llX\n", cpumask);

    return 0;
}

static int add_route(void) {

    struct in_addr dest, gw, src;
    dest.s_addr = inet_addr("10.0.2.0");
    gw.s_addr = inet_addr("10.0.2.1");
    src.s_addr = inet_addr("10.0.2.20");
    
    int ret = route_net_add(&dest, 24, RTF_LOCALIN, &gw, dev_port, &src, 1500, 0);
    if (ret != 0) {
        printf("[ERR] route_add failed: %d\n", ret);
        return -1;
    }

    return 0;
}

// Add this helper function
int add_real_server_to_service(struct dp_vs_service* svc) {
    struct dp_vs_dest_conf dest;
    memset(&dest, 0, sizeof(dest));
    dest.af = AF_INET;
    dest.proto = IPPROTO_TCP;
    dest.port = htons(8000);
    dest.addr.in.s_addr = inet_addr("10.0.2.100");
    dest.weight = 1;
    // dest.fwdmode = DPVS_FWD_MODE_SNAT; // SNAT (masquerading)
    dest.fwdmode = DPVS_FWD_MODE_FNAT;

    int ret = dp_vs_dest_add(svc, &dest);
    if (ret != 0) {
        printf("Failed to add real server: %d\n", ret);
        return -1;
    }
    printf("Real server 10.0.2.100:8000 added to service with FNAT.\n");
    return 0;
}

int add_dpvs_virtual_service() {
    struct dp_vs_service_conf svc;
    memset(&svc, 0, sizeof(svc));

    svc.af = AF_INET;
    svc.proto = IPPROTO_TCP;
    svc.port = htons(8000);
    svc.addr.in.s_addr = inet_addr("10.0.2.200");
    strncpy(svc.sched_name, "rr", DP_VS_SCHEDNAME_MAXLEN - 1);
    svc.sched_name[DP_VS_SCHEDNAME_MAXLEN - 1] = '\0';

    // Set other fields as needed, e.g. timeout, flags, etc.
    svc.flags = 0;
    svc.timeout = 0;
    svc.netmask = 0xffffffff;

    struct dp_vs_service *svc_ptr = NULL;
    int ret = dp_vs_service_add(&svc, &svc_ptr, rte_lcore_id());
    if (ret != 0) {
        printf("[ERR] Failed to add service: %d\n", ret);
        return NULL;
    }
    printf("[INFO] Service created successfully!\n");

    // Add the real server immediately after service creation
    add_real_server_to_service(svc_ptr);

    // Add local address 10.0.2.20 to the service, bound to interface "dpdk0"
    union inet_addr laddr;
    laddr.in.s_addr = inet_addr("10.0.2.20");
    int laddr_ret = dp_vs_laddr_add(svc_ptr, AF_INET, &laddr, "dpdk0\0");
    if (laddr_ret != 0) {
        printf("[ERR] Failed to add local address: %d\n", laddr_ret);
    } else {
        printf("[INFO] Local address 10.0.2.20 added to service on dpdk0.\n");
    }

    return 0;
}


// static int add_route6(void) {
//     struct in6_addr dest, gw, src;
//     dest.s_addr = inet_pton(AF_INET6, "2001:db8:1234:5678::1", &dest);
//     gw.s_addr = inet_pton(AF_INET6, "2001:db8:1234:5678::2", &gw);
//     src.s_addr = inet_pton(AF_INET6, "2001:db8:1234:5678::3", &src);

//     int ret = route_net_add(&dest, 64, RTF_LOCALIN, &gw, dev_port, &src, 1500, 0);
//     if (ret != 0) {
//         printf("[ERR] route_add failed: %d\n", ret);
//         return -1;
//     }

//     return 0;
// }


// Initialize DPDK environment and mbuf pool
static int init_dpdk(void) {
    int err, nports;
    char pool_name[32];
    char *argv[5];
    struct timeval tv;
    portid_t pid;

    char pql_conf_buf[LCORE_CONF_BUFFER_LEN];
    int pql_conf_buf_len = LCORE_CONF_BUFFER_LEN;

    printf("Initializing DPDK environment\n");
    argv[0] = (char *)malloc(10);
    memset(argv[0], 0, 10);
    strcpy(argv[0], "dpvs\0");

    // parse arguments
    if (!dpvs_ipc_file)
        dpvs_ipc_file="/var/run/dpvs.ipc";

    if (!dpvs_conf_file)
        dpvs_conf_file="/home/lab/Desktop/side-projects/linux-setup/dpvs-fuzzer/fuzz/dpvs.conf";

    if (!dpvs_pid_file)
        dpvs_pid_file="/home/lab/Desktop/side-projects/linux-setup/dpvs-fuzzer/fuzz/dpvs.pid";

    // 
    dpvs_state_set(DPVS_STATE_INIT);

    gettimeofday(&tv, NULL);
    srandom(tv.tv_sec ^ tv.tv_usec ^ getpid());
    srand48(tv.tv_sec ^ tv.tv_usec ^ getpid());
    rte_srand((uint64_t)(tv.tv_sec ^ tv.tv_usec ^ getpid()));
    sys_start_time();

    if (get_numa_nodes() > DPVS_MAX_SOCKET) {
        fprintf(stderr, "DPVS_MAX_SOCKET is smaller than system numa nodes!\n");
        return -1;
    }

    if (set_all_thread_affinity() != 0) {
        fprintf(stderr, "set_all_thread_affinity failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize EAL
    printf("Initializing EAL\n");
    err = rte_eal_init(1, argv);
    if (err < 0){
        printf("[ERR] Invalid EAL parameters\n");
        return -1;
    }

    RTE_LOG(INFO, DPVS, "dpvs version: %s, build on %s\n", DPVS_VERSION, DPVS_BUILD_DATE);
    RTE_LOG(INFO, DPVS, "dpvs-conf-file: %s\n", dpvs_conf_file);
    RTE_LOG(INFO, DPVS, "dpvs-pid-file: %s\n", dpvs_pid_file);
    RTE_LOG(INFO, DPVS, "dpvs-ipc-file: %s\n", dpvs_ipc_file);

    rte_timer_subsystem_init();
    
    // init modules
    if (!modules_init()) {
        return -1;
    }

    // Create a test network interface
    printf("[INFO] Config and start dpdk port\n");
    dev_port = netif_alloc(NETIF_PORT_ID_INVALID, 0, "dpdk0", 1, 1, NULL);
    if (!dev_port) {
        return -1;
    }
    
    // Set up basic port configuration
    printf("Setting up basic port configuration\n");
    dev_port->mbuf_pool = pktmbuf_pool[0];
    dev_port->flag |= NETIF_PORT_FLAG_ENABLED | NETIF_PORT_FLAG_FORWARD2KNI;
    dev_port->mtu = 1500;
    memcpy(dev_port->name, "dpdk0\0", 6);

    // Assign IP address to dev_port
    printf("[INFO] Assigning IP address to dev_port\n");
    union inet_addr ip;
    memset(&ip, 0, sizeof(ip));
    ip.in.s_addr = inet_addr("10.0.2.20");

    int ret_inet = inet_addr_add(AF_INET, dev_port, &ip, 24, NULL, 0, 0, 0, 0);
    if (ret_inet != 0) {
        printf("[ERR] Failed to assign IP address to dev_port: %d\n", ret_inet);
        return -1;
    }


    /* print port-queue-lcore relation */
    netif_print_lcore_conf(pql_conf_buf, &pql_conf_buf_len, true, 0);
    RTE_LOG(INFO, DPVS, "port-queue-lcore relation array: \n%s\n",
            pql_conf_buf);

    // /* start slave worker threads */ // <--- pay attention for racing condition
    // dpvs_lcore_start(0);

    // /* start async logging worker thread */
    // log_slave_init();

    /* write pid file */
    if (!pidfile_write(dpvs_pid_file, getpid()))
        return -1;

    dpvs_state_set(DPVS_STATE_NORMAL);

    // /* start control plane thread loop */
    // dpvs_lcore_start(1);

    // ending

    return 0;
}

void fill_eth_hdr(struct rte_ether_hdr *eth){
    char rd_eth_src[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    char rd_eth_dst[6] = {0x77, 0x66, 0x55, 0x44, 0x33, 0x22};

    eth->ether_type = htons(ETH_TYPE_IPV4);
    // copy any 6 bytes address for source and destination
    memcpy((char *)&eth->dst_addr, rd_eth_dst, 6);
    memcpy((char *)&eth->src_addr, rd_eth_src, 6);
}

void fill_eth6_hdr(struct rte_ether_hdr *eth){
    char rd_eth_src[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    char rd_eth_dst[6] = {0x77, 0x66, 0x55, 0x44, 0x33, 0x22};

    eth->ether_type = htons(ETH_TYPE_IPV6);
    memcpy((char *)&eth->dst_addr, rd_eth_dst, 6);
    memcpy((char *)&eth->src_addr, rd_eth_src, 6);
}

void fill_ip4_src_addr(rte_be32_t *sai, enum dpvs::SrcAddr addr) {
    // TODO: fill ip4 addr
    switch (addr) {
        case dpvs::IN4_SRC_ADDR_0:
            // 10.0.2.1
            *sai = 0x0102000a;
            break;
        case dpvs::IN4_SRC_ADDR_BROADCAST:
            // 255.255.255.255
            *sai = 0xffffffff;
            break;
    }
}

void fill_ip4_dst_addr(rte_be32_t *sai, enum dpvs::DstAddr addr) {
    // TODO: fill ip4 addr
    switch (addr) {
        case dpvs::IN4_DST_ADDR_0:
            // 10.0.2.200
            *sai = 0xc802000a;
            break;
        case dpvs::IN4_DST_ADDR_BROADCAST:
            // 255.255.255.255
            *sai = 0xffffffff;
            break;
    }
}

void fill_ip4_hdr(const dpvs::Ip4Hdr& ip4_hdr_proto, struct rte_ipv4_hdr *iph, int proto) {

    iph->version_ihl = ((ip4_hdr_proto.ip_vrs() & 0xF) << 4) | (ip4_hdr_proto.ip_hl() & 0xF);
    iph->type_of_service = ip4_hdr_proto.ip_tos() & 0xff;
    iph->total_length = htons(ip4_hdr_proto.ip_len() & 0xffff);
    iph->packet_id = htons(ip4_hdr_proto.ip_id() & 0xffff);
    iph->fragment_offset = htons(ip4_hdr_proto.ip_off() & 0xffff);
    printf("[INFO] ip_fragment_offset: %x\n", iph->fragment_offset);

    iph->time_to_live = ip4_hdr_proto.ip_ttl() & 0xff;

    if (proto != -1) {
        iph->next_proto_id = proto;
    } else {
        iph->next_proto_id = ip4_hdr_proto.ip_p() & 0xff;
    }
    iph->hdr_checksum = 0;
    rte_be32_t src_addr, dst_addr;
    fill_ip4_src_addr(&src_addr, ip4_hdr_proto.ip_src());
    fill_ip4_dst_addr(&dst_addr, ip4_hdr_proto.ip_dst());
    iph->src_addr = src_addr;
    iph->dst_addr = dst_addr;

    uint16_t hlen = (iph->version_ihl & 0xf) * 4;
    iph->hdr_checksum = rte_raw_cksum(iph, hlen);
    iph->hdr_checksum = ~iph->hdr_checksum;
    printf("[INFO] ip_hdr_checksum: %x\n", iph->hdr_checksum);

}

void fill_tcp_hdr(const dpvs::TcpHdr& tcp_hdr_proto, struct rte_tcp_hdr *tcp_hdr) {
    tcp_hdr->src_port = htons(tcp_hdr_proto.th_sport() & 0xffff);
    tcp_hdr->dst_port = htons(tcp_hdr_proto.th_dport() & 0xffff);
    tcp_hdr->sent_seq = htonl(tcp_hdr_proto.th_seq());
    tcp_hdr->recv_ack = htonl(tcp_hdr_proto.th_ack());
    tcp_hdr->data_off = tcp_hdr_proto.th_off() & 0xff;
    // Combine all TCP flags into a single uint8_t
    uint8_t flags = 0;
    for (int i = 0; i < tcp_hdr_proto.th_flags_size(); i++) {
        flags |= tcp_hdr_proto.th_flags(i);
    }
    tcp_hdr->tcp_flags = flags;
    tcp_hdr->rx_win = htons(tcp_hdr_proto.th_win() & 0xffff);
    tcp_hdr->cksum = 0;
    tcp_hdr->tcp_urp = htons(tcp_hdr_proto.th_urp() & 0xffff);

    // TODO: calculate checksum of tcp header
}

void fill_in6_addr(struct rte_ipv6_addr *sai, enum dpvs::In6Addr addr) {
  memset(sai, 0, sizeof(struct rte_ipv6_addr));
  switch (addr) {
    case dpvs::IN6_ADDR_SELF: {
      sai->a[0] = 0xfe;
      sai->a[1] = 0x80;
      sai->a[2] = 0x00;
      sai->a[3] = 0x01;
      sai->a[4] = 0x00;
      sai->a[5] = 0x00;
      sai->a[6] = 0x01;
      break;
    }
    case dpvs::IN6_ADDR_LINK_LOCAL: {
      sai->a[0] = 0xfe;
      sai->a[1] = 0x80;
      break;
    }
    case dpvs::IN6_ADDR_LOOPBACK: {
      sai->a[0] = 0x00;
      sai->a[1] = 0x00;
      sai->a[2] = 0x00;
      sai->a[3] = 0x00;
      sai->a[4] = 0x00;
      sai->a[5] = 0x00;
      sai->a[6] = 0x00;
      sai->a[7] = 0x00;
      break;
    }
    case dpvs::IN6_ADDR_REAL:
    case dpvs::MAYBE_LOCALHOST: {
      sai->a[0] = 0x00;
      sai->a[1] = 0x00;
      sai->a[2] = 0x00;
      sai->a[3] = 0x00;
      sai->a[4] = 0x00;
      sai->a[5] = 0x00;
      sai->a[6] = 0x00;
      sai->a[7] = (uint8_t)addr;
      break;
    }
    case dpvs::IN6_ADDR_V4COMPAT: {
      sai->a[12] = 1;
    //   assert(IN6_IS_ADDR_V4COMPAT(sai));
      break;
    }
    case dpvs::IN6_ADDR_V4MAPPED: {
      *(uint32_t *)&sai->a[8] = 0xffff0000;
    //   assert(IN6_IS_ADDR_V4MAPPED(sai));
      break;
    }
    case dpvs::IN6_ADDR_6TO4: {
      sai->a[0] = 0x20;
      sai->a[1] = 0x02;
    //   assert(IN6_IS_ADDR_6TO4(sai));
      break;
    }
    case dpvs::IN6_ADDR_LINKLOCAL: {
      sai->a[0] = 0xfe;
      sai->a[1] = 0x80;
    //   assert(IN6_IS_ADDR_LINKLOCAL(sai));
      break;
    }
    case dpvs::IN6_ADDR_SITELOCAL: {
      sai->a[0] = 0xfe;
      sai->a[1] = 0xc0;
    //   assert(IN6_IS_ADDR_SITELOCAL(sai));
      break;
    }
    case dpvs::IN6_ADDR_MULTICAST: {
      sai->a[0] = 0xff;
    //   assert(IN6_IS_ADDR_MULTICAST(sai));
      break;
    }
    case dpvs::IN6_ADDR_UNIQUE_LOCAL: {
      sai->a[0] = 0xfc;
    //   assert(IN6_IS_ADDR_UNIQUE_LOCAL(sai));
      break;
    }
    case dpvs::IN6_ADDR_MC_NODELOCAL: {
      sai->a[0] = 0xff;
      sai->a[1] = IPV6_ADDR_SCOPE_NODELOCAL;
    //   assert(IN6_IS_ADDR_MC_NODELOCAL(sai));
      break;
    }
    case dpvs::IN6_ADDR_MC_INTFACELOCAL: {
      sai->a[0] = 0xff;
      sai->a[1] = 0x01;
    //   IPV6_ADDR_SCOPE_INTFACELOCAL
    //   assert(IN6_IS_ADDR_MC_INTFACELOCAL(sai));
      break;
    }
    case dpvs::IN6_ADDR_MC_LINKLOCAL: {
      sai->a[0] = 0xff;
      sai->a[1] = IPV6_ADDR_SCOPE_LINKLOCAL;
    //   assert(IN6_IS_ADDR_MC_LINKLOCAL(sai));
      break;
    }
    case dpvs::IN6_ADDR_MC_SITELOCAL: {
      sai->a[0] = 0xff;
      sai->a[1] = IPV6_ADDR_SCOPE_SITELOCAL;
    //   assert(IN6_IS_ADDR_MC_SITELOCAL(sai));
      break;
    }
    case dpvs::IN6_ADDR_MC_ORGLOCAL: {
      sai->a[0] = 0xff;
      sai->a[1] = IPV6_ADDR_SCOPE_ORGLOCAL;
    //   assert(IN6_IS_ADDR_MC_ORGLOCAL(sai));
      break;
    }
    case dpvs::IN6_ADDR_MC_GLOBAL: {
      sai->a[0] = 0xff;
      sai->a[1] = IPV6_ADDR_SCOPE_GLOBAL;
    //   assert(IN6_IS_ADDR_MC_GLOBAL(sai));
      break;
    }
    case dpvs::IN6_ADDR_UNSPECIFIED:
    case dpvs::IN6_ADDR_ANY: {
    //   assert(IN6_IS_ADDR_UNSPECIFIED(sai));
      break;
    }
    case dpvs::IN6_ADDR_LOCAL_ADDRESS: {
      // Discovered this address dynamically
      // fe80:0001:0000:0000:a8aa:aaaa:aaaa:aaaa
      sai->a[0] = 0xfe;
      sai->a[1] = 0x80;
      sai->a[2] = 0x00;
      sai->a[3] = 0x01;
      sai->a[4] = 0x00;
      sai->a[5] = 0x00;
      sai->a[6] = 0x00;
      sai->a[7] = 0x00;
      sai->a[8] = 0xa8;
      sai->a[9] = 0xaa;
      sai->a[10] = 0xaa;
      sai->a[11] = 0xaa;
      sai->a[12] = 0xaa;
      sai->a[13] = 0xaa;
      sai->a[14] = 0xaa;
      sai->a[15] = 0xaa;
    }
  }
}

void fill_ip6_hdr(const dpvs::Ip6Hdr& ip6_hdr_proto, struct ip6_hdr *ip6_hdr) {

    uint32_t flow_label = ip6_hdr_proto.flow_id();
    uint32_t tclass = ip6_hdr_proto.tclass();
    uint32_t version = ip6_hdr_proto.version();

    // /* 4 bits version, 8 bits TC, 20 bits flow-ID */
    ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_flow = (version << 28) | (tclass << 20) | (flow_label & 0xfffff);


    ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_plen = htons(ip6_hdr_proto.payload_len() & 0xffff);
    ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_nxt = ip6_hdr_proto.proto();
    ip6_hdr->ip6_ctlun.ip6_un1.ip6_un1_hlim = ip6_hdr_proto.hop_limit() & 0xff;

    // /* 4 bits version, top 4 bits tclass */
    ip6_hdr->ip6_ctlun.ip6_un2_vfc = (version << 4) | (tclass & 0xf);

    fill_in6_addr((struct rte_ipv6_addr *)&ip6_hdr->ip6_src, ip6_hdr_proto.src_addr());
    fill_in6_addr((struct rte_ipv6_addr *)&ip6_hdr->ip6_dst, ip6_hdr_proto.dst_addr());
}

struct rte_mbuf *DoTcpPacket(const dpvs::TcpPacket& tcp_packet) {

    struct rte_mbuf *mbuf = rte_pktmbuf_alloc(pktmbuf_pool[0]);
    uint8_t *data;
    if (!mbuf) {
        return NULL;
    }

    mbuf_userdata_reset(mbuf);

    mbuf->port = 0;
    mbuf->nb_segs = 1;
    mbuf->ol_flags = 0;

    // fill ethernet header
    data = rte_pktmbuf_mtod(mbuf, uint8_t *);
    fill_eth_hdr((struct rte_ether_hdr *)data);

    // fill ip header
    data += sizeof(struct rte_ether_hdr);
    fill_ip4_hdr(tcp_packet.ip4_hdr(), (struct rte_ipv4_hdr *)data, IPPROTO_TCP);
    
    // fill tcp header
    data += sizeof(struct rte_ipv4_hdr);
    fill_tcp_hdr(tcp_packet.tcp_hdr(), (struct rte_tcp_hdr *)data);
    
    // optional data 
    if (tcp_packet.has_data()) {
        data += sizeof(struct rte_tcp_hdr);
        memcpy(data, tcp_packet.data().data(), tcp_packet.data().size());
    }

    // adjust len
    mbuf->data_off = 128;
    mbuf->data_len = tcp_packet.data().size() + sizeof(struct rte_tcp_hdr) + sizeof(struct rte_ipv4_hdr) + sizeof(struct rte_ether_hdr);
    mbuf->pkt_len = mbuf->data_len;
    
    return mbuf;
}

struct rte_mbuf *DoIp6Packet(const dpvs::Ip6Packet& ip6_packet) {

    struct rte_mbuf *mbuf = rte_pktmbuf_alloc(pktmbuf_pool[0]);
    uint8_t *data;
    if (!mbuf) {
        return NULL;
    }

    mbuf_userdata_reset(mbuf);
    mbuf->port = 0;
    mbuf->nb_segs = 1;
    mbuf->ol_flags = 0;

    // fill ethernet header
    data = rte_pktmbuf_mtod(mbuf, uint8_t *);
    fill_eth6_hdr((struct rte_ether_hdr *)data);

    // fill ip6 header
    data += sizeof(struct rte_ether_hdr);
    fill_ip6_hdr(ip6_packet.ip6_hdr(), (struct ip6_hdr *)data);


    // optional data
    if (ip6_packet.has_data()) {
        data += sizeof(struct ip6_hdr);
        memcpy(data, ip6_packet.data().data(), ip6_packet.data().size());
    }

    // adjust len
    mbuf->data_off = 128;
    mbuf->data_len = ip6_packet.data().size() + sizeof(struct ip6_hdr) + sizeof(struct rte_ether_hdr);
    mbuf->pkt_len = mbuf->data_len;

    return mbuf;
}

struct rte_mbuf *DoTcp6Packet(const dpvs::Tcp6Packet& tcp6_packet) {

    struct rte_mbuf *mbuf = rte_pktmbuf_alloc(pktmbuf_pool[0]);
    uint8_t *data;
    if (!mbuf) {
        return NULL;
    }

    mbuf_userdata_reset(mbuf);
    mbuf->port = 0;
    mbuf->nb_segs = 1;
    mbuf->ol_flags = 0;

    // fill ethernet header
    data = rte_pktmbuf_mtod(mbuf, uint8_t *);
    fill_eth6_hdr((struct rte_ether_hdr *)data);

    // fill ip6 header
    data += sizeof(struct rte_ether_hdr);
    fill_ip6_hdr(tcp6_packet.ip6_hdr(), (struct ip6_hdr *)data);

    // fill tcp header
    data += sizeof(struct ip6_hdr);
    fill_tcp_hdr(tcp6_packet.tcp_hdr(), (struct rte_tcp_hdr *)data);

    // optional data
    if (tcp6_packet.has_data()) {
        data += sizeof(struct rte_tcp_hdr);
        memcpy(data, tcp6_packet.data().data(), tcp6_packet.data().size());
    }

    // adjust len
    mbuf->data_off = 128;
    mbuf->data_len = tcp6_packet.data().size() + sizeof(struct rte_tcp_hdr) + sizeof(struct ip6_hdr) + sizeof(struct rte_ether_hdr);
    mbuf->pkt_len = mbuf->data_len;

    return mbuf;
}

struct rte_mbuf *DoIp4Packet(const dpvs::Ip4Packet& ip4_packet) {

    struct rte_mbuf *mbuf = rte_pktmbuf_alloc(pktmbuf_pool[0]);
    uint8_t *data;
    if (!mbuf) {
        return NULL;
    }

    mbuf_userdata_reset(mbuf);
    mbuf->port = 0;
    mbuf->nb_segs = 1;
    mbuf->ol_flags = 0;

    // fill ethernet header
    data = rte_pktmbuf_mtod(mbuf, uint8_t *);
    fill_eth_hdr((struct rte_ether_hdr *)data);

    // fill ip header
    data += sizeof(struct rte_ether_hdr);
    fill_ip4_hdr(ip4_packet.ip4_hdr(), (struct rte_ipv4_hdr *)data, IPPROTO_IP);

    // optional data
    if (ip4_packet.has_data()) {
        data += sizeof(struct rte_ipv4_hdr);
        memcpy(data, ip4_packet.data().data(), ip4_packet.data().size());
    }

    mbuf->data_off = 128;
    mbuf->data_len = ip4_packet.data().size() + sizeof(struct rte_ipv4_hdr) + sizeof(struct rte_ether_hdr);
    mbuf->pkt_len = mbuf->data_len;
    mbuf->packet_type = RTE_PTYPE_L3_IPV4;
    printf("[INFO] mbuf: %p\n", mbuf);

    return mbuf;
}

void fill_icmp_hdr(const dpvs::IcmpHdr& icmp_hdr, struct rte_icmp_hdr *icmp_hdr_proto) {

    icmp_hdr_proto->icmp_type = icmp_hdr.icmp_type();
    icmp_hdr_proto->icmp_code = icmp_hdr.icmp_code();
    icmp_hdr_proto->icmp_ident = icmp_hdr.icmp_id() & 0xffff;
    icmp_hdr_proto->icmp_seq_nb = icmp_hdr.icmp_seq() & 0xffff;

    // calculate checksum 
    icmp_hdr_proto->icmp_cksum = 0;
    icmp_hdr_proto->icmp_cksum = rte_raw_cksum(icmp_hdr_proto, sizeof(struct rte_icmp_hdr));
    icmp_hdr_proto->icmp_cksum = ~icmp_hdr_proto->icmp_cksum;
    printf("[INFO] icmp_hdr_proto->icmp_cksum: 0x%x\n", icmp_hdr_proto->icmp_cksum);
}

// construct icmp packet
struct rte_mbuf *DoIcmpPacket(const dpvs::IcmpPacket& icmp_packet) {

    struct rte_mbuf *mbuf = rte_pktmbuf_alloc(pktmbuf_pool[0]);
    uint8_t *data;

    if (!mbuf) {
        return NULL;
    }

    mbuf_userdata_reset(mbuf);
    mbuf->port = 0;
    mbuf->nb_segs = 1;
    mbuf->ol_flags = 0;

    // fill ethernet header
    data = rte_pktmbuf_mtod(mbuf, uint8_t *);
    fill_eth_hdr((struct rte_ether_hdr *)data);

    // fill ip header
    data += sizeof(struct rte_ether_hdr);
    fill_ip4_hdr(icmp_packet.ip4_hdr(), (struct rte_ipv4_hdr *)data, IPPROTO_ICMP);

    // fill icmp header
    data += sizeof(struct rte_ipv4_hdr);
    fill_icmp_hdr(icmp_packet.icmp_hdr(), (struct rte_icmp_hdr *)data);

    // optional data
    if (icmp_packet.has_data()) {
        data += sizeof(struct rte_icmp_hdr);
        memcpy(data, icmp_packet.data().data(), icmp_packet.data().size());
    }

    // adjust len
    mbuf->data_off = 128;
    mbuf->data_len = icmp_packet.data().size() + sizeof(struct rte_icmp_hdr) + sizeof(struct rte_ipv4_hdr) + sizeof(struct rte_ether_hdr);
    mbuf->pkt_len = mbuf->data_len;

    return mbuf;
}



struct rte_mbuf *create_mbuf_from_proto(const dpvs::Mbuf& mbuf_proto) {

    // no-fill optional vlan tag
    switch (mbuf_proto.mbuf_case()) {
        case dpvs::Mbuf::kTcpPacket:
            // pink color
            printf("\033[1;35m[INFO] TCP packet\033[0m\n");
            return DoTcpPacket(mbuf_proto.tcp_packet());
        case dpvs::Mbuf::kTcp6Packet:
            // blue color
            printf("\033[1;34m[INFO] TCP6 packet\033[0m\n");
            return DoTcp6Packet(mbuf_proto.tcp6_packet());
        case dpvs::Mbuf::kIp4Packet:
            // purple color
            printf("\033[1;35m[INFO] IP4 packet\033[0m\n");
            return DoIp4Packet(mbuf_proto.ip4_packet());
        case dpvs::Mbuf::kIp6Packet:
            // cyan color
            printf("\033[1;36m[INFO] IP6 packet\033[0m\n");
            return DoIp6Packet(mbuf_proto.ip6_packet());
        case dpvs::Mbuf::kIcmpPacket:
            // yellow color
            printf("\033[1;33m[INFO] ICMP packet\033[0m\n");
            return DoIcmpPacket(mbuf_proto.icmp_packet());
        default:
            return NULL;
    }

}

#if defined(LLVM_BATCH_TEST)

int main(int argc, char **argv) {
    static bool initialized = false;
    struct rte_mbuf *mbuf;
    int err;
    dpvs::Mbuf proto;
    
    printf("Begining normal main\n");
    if (!initialized) {
        if (init_dpdk() < 0) {
            return 1;
        }

        initialized = true;
    }

    // adding more later

    return 0;
}

#else

static int first_lcore(void *arg) {
    static struct rte_mbuf *mbuf = NULL;
    dpvs::Mbuf proto = *(dpvs::Mbuf *)arg;
    static bool routing_enabled = false;

    if (!routing_enabled) {
        if (add_dpvs_virtual_service() < 0) {
            return -1;
        }
        if (add_route() < 0) {
            return -11;
        }
        routing_enabled = true;
    }

    printf("\033[1;33mCreating mbuf from protobuf message\033[0m\n");
    mbuf = create_mbuf_from_proto(proto);
    if (!mbuf) {
        return -1;
    }
    
    // print with green color, and bold
    printf("\033[1;32mCalling netif_rcv_mbuf\033[0m\n");
    // Call the target function
    netif_rcv_mbuf(dev_port, 1, mbuf, false);

    // yellow color
    printf("\033[1;33mFreeing mbuf\033[0m\n");
    rte_pktmbuf_free(mbuf);
    return 0;
}

// Main fuzzer function
DEFINE_BINARY_PROTO_FUZZER(const dpvs::Mbuf& proto) {
    
    printf("Begining fuzzing\n");
    if (!initialized) {
        if (init_dpdk() < 0) {
            return;
        }

        initialized = true;
    }

    // switch to lcore 1
    rte_eal_remote_launch(first_lcore, (void *)&proto, 1);

    // wait for all cores to finish
    rte_eal_wait_lcore(1);
}

#endif // LLVM_FUZZER

