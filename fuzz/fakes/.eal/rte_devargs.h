#ifndef _RTE_DEVARGS_H_
#define _RTE_DEVARGS_H_

#include <stdio.h>
#include "rte_dev.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTE_DEVARGS_KEY_BUS "bus"
#define RTE_DEVARGS_KEY_CLASS "class"
#define RTE_DEVARGS_KEY_DRIVER "driver"

struct rte_bus;

enum rte_devtype {
    RTE_DEVTYPE_ALLOWED,
    RTE_DEVTYPE_BLOCKED,
    RTE_DEVTYPE_VIRTUAL,
};

struct rte_devargs {
    void *next;
    enum rte_devtype type;
    int policy;
    char name[256];
    union {
        const char *args;
        const char *drv_str;
    };
    struct rte_bus *bus;
    void *cls;
    const char *bus_str;
    const char *cls_str;
    char *data;
};

int rte_devargs_parse(struct rte_devargs *da, const char *dev);
int rte_devargs_parsef(struct rte_devargs *da, const char *format, ...);
void rte_devargs_reset(struct rte_devargs *da);
int rte_devargs_insert(struct rte_devargs **da);
int rte_devargs_add(enum rte_devtype devtype, const char *devargs_str);
int rte_devargs_remove(struct rte_devargs *devargs);
unsigned int rte_devargs_type_count(enum rte_devtype devtype);
void rte_devargs_dump(FILE *f);
struct rte_devargs *rte_devargs_next(const char *busname, const struct rte_devargs *start);

#define RTE_EAL_DEVARGS_FOREACH(busname, da) \
    for (da = rte_devargs_next(busname, NULL); \
         da != NULL; \
         da = rte_devargs_next(busname, da))

#ifdef __cplusplus
}
#endif

#endif /* _RTE_DEVARGS_H_ */ 