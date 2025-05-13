#include "rte_lcore.h"


unsigned int rte_get_main_lcore(void) {
    // do nothing
    return 0;
}

int rte_lcore_has_role(unsigned int lcore_id, enum rte_lcore_role_t role) {
    // do nothing
    return 0;
}


unsigned int rte_lcore_count(void) {
    // do nothing
    return 0;
}


int rte_lcore_index(int lcore_id) {
    // do nothing
    return 0;
}

unsigned int rte_socket_id(void) {
    // do nothing
    return 0;
}

unsigned int rte_socket_count(void) {
    // do nothing
    return 0;
}

int rte_socket_id_by_idx(unsigned int idx) {
    // do nothing
    return 0;
}

unsigned int rte_lcore_to_socket_id(unsigned int lcore_id) {
    // do nothing
    return 0;
}


int rte_lcore_is_enabled(unsigned int lcore_id) {
    // do nothing
    return 0;
}

unsigned int rte_get_next_lcore(unsigned int i, int skip_main, int wrap) {
    // do nothing
    return 0;
}

void *rte_lcore_callback_register(const char *name, rte_lcore_init_cb init, rte_lcore_uninit_cb uninit, void *arg) {
    // do nothing
    return NULL;
}

void rte_lcore_callback_unregister(void *handle) {
    // do nothing
}

int rte_lcore_iterate(rte_lcore_iterate_cb cb, void *arg) {
    // do nothing
    return 0;
}

void rte_lcore_register_usage_cb(rte_lcore_usage_cb cb) {
    // do nothing
}

void rte_lcore_dump(FILE *f) {
    // do nothing
}

int rte_thread_register(void) {
    // do nothing
    return 0;
}

void rte_thread_unregister(void) {
    // do nothing
}


