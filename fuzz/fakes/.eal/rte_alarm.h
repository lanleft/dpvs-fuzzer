#ifndef _RTE_ALARM_H_
#define _RTE_ALARM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*rte_eal_alarm_callback)(void *arg);

int rte_eal_alarm_set(uint64_t us, rte_eal_alarm_callback cb, void *cb_arg);
int rte_eal_alarm_cancel(rte_eal_alarm_callback cb_fn, void *cb_arg);

#ifdef __cplusplus
}
#endif

#endif /* _RTE_ALARM_H_ */ 