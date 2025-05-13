#ifndef _RTE_SERVICE_H_
#define _RTE_SERVICE_H_

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RTE_SERVICE_NAME_MAX 32
#define RTE_SERVICE_CAP_MT_SAFE (1 << 0)
#define RTE_SERVICE_ATTR_CYCLES 0
#define RTE_SERVICE_ATTR_CALL_COUNT 1
#define RTE_SERVICE_ATTR_IDLE_CALL_COUNT 2
#define RTE_SERVICE_ATTR_ERROR_CALL_COUNT 3
#define RTE_SERVICE_LCORE_ATTR_LOOPS 0
#define RTE_SERVICE_LCORE_ATTR_CYCLES 1

uint32_t rte_service_get_count(void);
int32_t rte_service_get_by_name(const char *name, uint32_t *service_id);
const char *rte_service_get_name(uint32_t id);
int32_t rte_service_probe_capability(uint32_t id, uint32_t capability);
int32_t rte_service_map_lcore_set(uint32_t service_id, uint32_t lcore, uint32_t enable);
int32_t rte_service_map_lcore_get(uint32_t service_id, uint32_t lcore);
int32_t rte_service_runstate_set(uint32_t id, uint32_t runstate);
int32_t rte_service_runstate_get(uint32_t id);
int32_t rte_service_may_be_active(uint32_t id);
int32_t rte_service_set_runstate_mapped_check(uint32_t id, int32_t enable);
int32_t rte_service_run_iter_on_app_lcore(uint32_t id, uint32_t serialize_multithread_unsafe);
int32_t rte_service_lcore_start(uint32_t lcore_id);
int32_t rte_service_lcore_stop(uint32_t lcore_id);
int32_t rte_service_lcore_may_be_active(uint32_t lcore_id);
int32_t rte_service_lcore_add(uint32_t lcore);
int32_t rte_service_lcore_del(uint32_t lcore);
int32_t rte_service_lcore_count(void);
int32_t rte_service_lcore_reset_all(void);
int32_t rte_service_set_stats_enable(uint32_t id, int32_t enable);
int32_t rte_service_lcore_list(uint32_t array[], uint32_t n);
int32_t rte_service_lcore_count_services(uint32_t lcore);
int32_t rte_service_dump(FILE *f, uint32_t id);
int32_t rte_service_attr_get(uint32_t id, uint32_t attr_id, uint64_t *attr_value);
int32_t rte_service_attr_reset_all(uint32_t id);
int32_t rte_service_lcore_attr_get(uint32_t lcore, uint32_t attr_id, uint64_t *attr_value);
int32_t rte_service_lcore_attr_reset_all(uint32_t lcore);

#ifdef __cplusplus
}
#endif

#endif /* _RTE_SERVICE_H_ */ 