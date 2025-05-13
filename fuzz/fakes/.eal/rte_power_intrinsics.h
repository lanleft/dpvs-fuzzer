#ifndef _RTE_POWER_INTRINSIC_H_
#define _RTE_POWER_INTRINSIC_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RTE_POWER_MONITOR_OPAQUE_SZ 4

typedef int (*rte_power_monitor_clb_t)(const uint64_t val, const uint64_t opaque[RTE_POWER_MONITOR_OPAQUE_SZ]);

struct rte_power_monitor_cond {
    volatile void *addr;
    uint8_t size;
    rte_power_monitor_clb_t fn;
    uint64_t opaque[RTE_POWER_MONITOR_OPAQUE_SZ];
};

int rte_power_monitor(const struct rte_power_monitor_cond *pmc, const uint64_t tsc_timestamp);
int rte_power_monitor_wakeup(const unsigned int lcore_id);
int rte_power_pause(const uint64_t tsc_timestamp);
int rte_power_monitor_multi(const struct rte_power_monitor_cond pmc[], const uint32_t num, const uint64_t tsc_timestamp);

#ifdef __cplusplus
}
#endif

#endif /* _RTE_POWER_INTRINSIC_H_ */ 