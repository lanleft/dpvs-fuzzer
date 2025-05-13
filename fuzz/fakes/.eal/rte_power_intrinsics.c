#include "rte_power_intrinsics.h"

int rte_power_monitor(const struct rte_power_monitor_cond *pmc, const uint64_t tsc_timestamp) { return 0; }
int rte_power_monitor_wakeup(const unsigned int lcore_id) { return 0; }
int rte_power_pause(const uint64_t tsc_timestamp) { return 0; }
int rte_power_monitor_multi(const struct rte_power_monitor_cond pmc[], const uint32_t num, const uint64_t tsc_timestamp) { return 0; } 