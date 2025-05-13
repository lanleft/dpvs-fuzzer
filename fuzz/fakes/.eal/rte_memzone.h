#ifndef _RTE_MEMZONE_H_
#define _RTE_MEMZONE_H_

#include <stdio.h>
#include "rte_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTE_MEMZONE_2MB            0x00000001
#define RTE_MEMZONE_1GB            0x00000002
#define RTE_MEMZONE_16MB           0x00000100
#define RTE_MEMZONE_16GB           0x00000200
#define RTE_MEMZONE_256KB          0x00010000
#define RTE_MEMZONE_256MB          0x00020000
#define RTE_MEMZONE_512MB          0x00040000
#define RTE_MEMZONE_4GB            0x00080000
#define RTE_MEMZONE_SIZE_HINT_ONLY 0x00000004
#define RTE_MEMZONE_IOVA_CONTIG    0x00100000
#define RTE_MEMZONE_NAMESIZE 32

struct rte_memzone {
    char name[RTE_MEMZONE_NAMESIZE];
    rte_iova_t iova;
    union {
        void *addr;
        uint64_t addr_64;
    };
    size_t len;
    uint64_t hugepage_sz;
    int32_t socket_id;
    uint32_t flags;
};

int rte_memzone_max_set(size_t max);
size_t rte_memzone_max_get(void);
const struct rte_memzone *rte_memzone_reserve(const char *name, size_t len, int socket_id, unsigned flags);
const struct rte_memzone *rte_memzone_reserve_aligned(const char *name, size_t len, int socket_id, unsigned flags, unsigned align);
const struct rte_memzone *rte_memzone_reserve_bounded(const char *name, size_t len, int socket_id, unsigned flags, unsigned align, unsigned bound);
int rte_memzone_free(const struct rte_memzone *mz);
const struct rte_memzone *rte_memzone_lookup(const char *name);
void rte_memzone_dump(FILE *f);
void rte_memzone_walk(void (*func)(const struct rte_memzone *, void *arg), void *arg);

#ifdef __cplusplus
}
#endif

#endif /* _RTE_MEMZONE_H_ */ 