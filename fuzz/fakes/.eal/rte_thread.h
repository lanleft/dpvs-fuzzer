#ifndef _RTE_THREAD_H_
#define _RTE_THREAD_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RTE_THREAD_NAME_SIZE 16
#define RTE_MAX_THREAD_NAME_LEN RTE_THREAD_NAME_SIZE
#define RTE_THREAD_INTERNAL_PREFIX "dpdk-"
#define RTE_THREAD_INTERNAL_NAME_SIZE 11

typedef struct { uintptr_t opaque_id; } rte_thread_t;
typedef uint32_t (*rte_thread_func) (void *arg);
enum rte_thread_priority {
    RTE_THREAD_PRIORITY_NORMAL = 0,
    RTE_THREAD_PRIORITY_REALTIME_CRITICAL = 1,
};
typedef struct {
    enum rte_thread_priority priority;
} rte_thread_attr_t;
typedef struct eal_tls_key *rte_thread_key;

int rte_thread_create(rte_thread_t *thread_id, const rte_thread_attr_t *thread_attr, rte_thread_func thread_func, void *arg);
int rte_thread_create_control(rte_thread_t *thread, const char *name, rte_thread_func thread_func, void *arg);
int rte_thread_create_internal_control(rte_thread_t *id, const char *name, rte_thread_func func, void *arg);
int rte_thread_join(rte_thread_t thread_id, uint32_t *value_ptr);
int rte_thread_detach(rte_thread_t thread_id);
rte_thread_t rte_thread_self(void);
void rte_thread_set_name(rte_thread_t thread_id, const char *thread_name);
void rte_thread_set_prefixed_name(rte_thread_t id, const char *name);
int rte_thread_equal(rte_thread_t t1, rte_thread_t t2);
int rte_thread_attr_init(rte_thread_attr_t *attr);
int rte_thread_attr_set_priority(rte_thread_attr_t *thread_attr, enum rte_thread_priority priority);
int rte_thread_get_priority(rte_thread_t thread_id, enum rte_thread_priority *priority);
int rte_thread_set_priority(rte_thread_t thread_id, enum rte_thread_priority priority);
int rte_thread_key_create(rte_thread_key *key, void (*destructor)(void *));
int rte_thread_key_delete(rte_thread_key key);
int rte_thread_value_set(rte_thread_key key, const void *value);
void *rte_thread_value_get(rte_thread_key key);

#ifdef __cplusplus
}
#endif

#endif /* _RTE_THREAD_H_ */ 