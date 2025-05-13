#include "rte_service.h"

uint32_t rte_service_get_count(void) { return 0; }
int32_t rte_service_get_by_name(const char *name, uint32_t *service_id) { return 0; }
const char *rte_service_get_name(uint32_t id) { return NULL; }
int32_t rte_service_probe_capability(uint32_t id, uint32_t capability) { return 0; }
int32_t rte_service_map_lcore_set(uint32_t service_id, uint32_t lcore, uint32_t enable) { return 0; }
int32_t rte_service_map_lcore_get(uint32_t service_id, uint32_t lcore) { return 0; }
int32_t rte_service_runstate_set(uint32_t id, uint32_t runstate) { return 0; }
int32_t rte_service_runstate_get(uint32_t id) { return 0; }
int32_t rte_service_may_be_active(uint32_t id) { return 0; }
int32_t rte_service_set_runstate_mapped_check(uint32_t id, int32_t enable) { return 0; }
int32_t rte_service_run_iter_on_app_lcore(uint32_t id, uint32_t serialize_multithread_unsafe) { return 0; }
int32_t rte_service_lcore_start(uint32_t lcore_id) { return 0; }
int32_t rte_service_lcore_stop(uint32_t lcore_id) { return 0; }
int32_t rte_service_lcore_may_be_active(uint32_t lcore_id) { return 0; }
int32_t rte_service_lcore_add(uint32_t lcore) { return 0; }
int32_t rte_service_lcore_del(uint32_t lcore) { return 0; }
int32_t rte_service_lcore_count(void) { return 0; }
int32_t rte_service_lcore_reset_all(void) { return 0; }
int32_t rte_service_set_stats_enable(uint32_t id, int32_t enable) { return 0; }
int32_t rte_service_lcore_list(uint32_t array[], uint32_t n) { return 0; }
int32_t rte_service_lcore_count_services(uint32_t lcore) { return 0; }
int32_t rte_service_dump(FILE *f, uint32_t id) { return 0; }
int32_t rte_service_attr_get(uint32_t id, uint32_t attr_id, uint64_t *attr_value) { return 0; }
int32_t rte_service_attr_reset_all(uint32_t id) { return 0; }
int32_t rte_service_lcore_attr_get(uint32_t lcore, uint32_t attr_id, uint64_t *attr_value) { return 0; }
int32_t rte_service_lcore_attr_reset_all(uint32_t lcore) { return 0; } 