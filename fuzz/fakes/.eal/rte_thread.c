#include <stddef.h>
#include "rte_thread.h"

int rte_thread_create(rte_thread_t *thread_id, const rte_thread_attr_t *thread_attr, rte_thread_func thread_func, void *arg) { return 0; }
int rte_thread_create_control(rte_thread_t *thread, const char *name, rte_thread_func thread_func, void *arg) { return 0; }
int rte_thread_create_internal_control(rte_thread_t *id, const char *name, rte_thread_func func, void *arg) { return 0; }
int rte_thread_join(rte_thread_t thread_id, uint32_t *value_ptr) { return 0; }
int rte_thread_detach(rte_thread_t thread_id) { return 0; }
rte_thread_t rte_thread_self(void) { rte_thread_t t = {0}; return t; }
void rte_thread_set_name(rte_thread_t thread_id, const char *thread_name) { }
void rte_thread_set_prefixed_name(rte_thread_t id, const char *name) { }
int rte_thread_equal(rte_thread_t t1, rte_thread_t t2) { return 0; }
int rte_thread_attr_init(rte_thread_attr_t *attr) { return 0; }
int rte_thread_attr_set_priority(rte_thread_attr_t *thread_attr, enum rte_thread_priority priority) { return 0; }
int rte_thread_get_priority(rte_thread_t thread_id, enum rte_thread_priority *priority) { return 0; }
int rte_thread_set_priority(rte_thread_t thread_id, enum rte_thread_priority priority) { return 0; }
int rte_thread_key_create(rte_thread_key *key, void (*destructor)(void *)) { return 0; }
int rte_thread_key_delete(rte_thread_key key) { return 0; }
int rte_thread_value_set(rte_thread_key key, const void *value) { return 0; }
void *rte_thread_value_get(rte_thread_key key) { return NULL; } 