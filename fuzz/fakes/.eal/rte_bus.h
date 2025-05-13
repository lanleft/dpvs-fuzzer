#ifndef _RTE_BUS_H_
#define _RTE_BUS_H_

#include <stdio.h>
#include "rte_eal.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rte_bus;
struct rte_device;

typedef int (*rte_bus_cmp_t)(const struct rte_bus *bus, const void *data);

const char *rte_bus_name(const struct rte_bus *bus);
int rte_bus_scan(void);
int rte_bus_probe(void);
void rte_bus_dump(FILE *f);
struct rte_bus *rte_bus_find(const struct rte_bus *start, rte_bus_cmp_t cmp, const void *data);
struct rte_bus *rte_bus_find_by_device(const struct rte_device *dev);
struct rte_bus *rte_bus_find_by_name(const char *busname);
enum rte_iova_mode rte_bus_get_iommu_class(void);

#ifdef __cplusplus
}
#endif

#endif /* _RTE_BUS_H_ */ 