#include "rte_devargs.h"

int rte_devargs_parse(struct rte_devargs *da, const char *dev) { return 0; }
int rte_devargs_parsef(struct rte_devargs *da, const char *format, ...) { return 0; }
void rte_devargs_reset(struct rte_devargs *da) { }
int rte_devargs_insert(struct rte_devargs **da) { return 0; }
int rte_devargs_add(enum rte_devtype devtype, const char *devargs_str) { return 0; }
int rte_devargs_remove(struct rte_devargs *devargs) { return 0; }
unsigned int rte_devargs_type_count(enum rte_devtype devtype) { return 0; }
void rte_devargs_dump(FILE *f) { }
struct rte_devargs *rte_devargs_next(const char *busname, const struct rte_devargs *start) { return 0; } 