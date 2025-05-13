#include "rte_bus.h"

const char *rte_bus_name(const struct rte_bus *bus) { return NULL; }
int rte_bus_scan(void) { return 0; }
int rte_bus_probe(void) { return 0; }
void rte_bus_dump(FILE *f) { }
struct rte_bus *rte_bus_find(const struct rte_bus *start, rte_bus_cmp_t cmp, const void *data) { return NULL; }
struct rte_bus *rte_bus_find_by_device(const struct rte_device *dev) { return NULL; }
struct rte_bus *rte_bus_find_by_name(const char *busname) { return NULL; }
enum rte_iova_mode rte_bus_get_iommu_class(void) { return RTE_IOVA_DC; } 