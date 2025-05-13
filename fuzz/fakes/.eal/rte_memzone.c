#include "rte_memzone.h"

int rte_memzone_max_set(size_t max) { return 0; }
size_t rte_memzone_max_get(void) { return 0; }
const struct rte_memzone *rte_memzone_reserve(const char *name, size_t len, int socket_id, unsigned flags) { return NULL; }
const struct rte_memzone *rte_memzone_reserve_aligned(const char *name, size_t len, int socket_id, unsigned flags, unsigned align) { return NULL; }
const struct rte_memzone *rte_memzone_reserve_bounded(const char *name, size_t len, int socket_id, unsigned flags, unsigned align, unsigned bound) { return NULL; }
int rte_memzone_free(const struct rte_memzone *mz) { return 0; }
const struct rte_memzone *rte_memzone_lookup(const char *name) { return NULL; }
void rte_memzone_dump(FILE *f) { }
void rte_memzone_walk(void (*func)(const struct rte_memzone *, void *arg), void *arg) { } 