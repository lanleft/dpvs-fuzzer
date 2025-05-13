// fake rte_kni.h

#ifndef __RTE_KNI_H__
#define __RTE_KNI_H__

#include <stdint.h>

#define RTE_KNI_NAMESIZE 32

struct netif_port;
struct rte_mbuf;

static inline void rte_kni_handle_request(struct netif_port *dev) {
    // do nothing
}

static inline unsigned rte_kni_rx_burst(struct netif_port *dev, struct rte_mbuf **pkts, unsigned int nb_pkts) {
    // do nothing
    return 0;
}

static inline unsigned rte_kni_tx_burst(struct netif_port *dev, struct rte_mbuf **pkts, unsigned int nb_pkts) {
    // do nothing
    return 0;
}

// rte_kni_get_name 
static inline const char *rte_kni_get_name(struct netif_port *dev) {
    // do nothing
    return "kni0";
}

// kni_del_dev
static inline int kni_del_dev(struct netif_port *dev) {
    // do nothing
    return 0;
}

// kni_add_dev
static inline int kni_add_dev(struct netif_port *dev, const char *kniname) {
    // do nothing
    return 0;
}

// kni_ctrl_term
static inline int kni_ctrl_term(void) {
    // do nothing
    return 0;
}

// kni_ctrl_init
static inline int kni_ctrl_init(void) {
    // do nothing
    return 0;
}

// kni_init
static inline int kni_init(void) {
    // do nothing
    return 0;
}







#endif // __RTE_KNI_H__