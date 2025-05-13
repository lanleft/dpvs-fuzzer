// fake some allocate functions

#include <stdlib.h>
#include "rte_random.h"

// rte_lcore_var_alloc
static inline void *rte_alloc(void *ptr) {
    return malloc(sizeof(ptr));
}

// #define RTE_LCORE_VAR_ALLOC(handle) rte_alloc(handle)