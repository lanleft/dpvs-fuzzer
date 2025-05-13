#include "rte_malloc.h"
#include <stdlib.h>

void rte_free(void *ptr) {
    free(ptr);
 }
void *rte_malloc(const char *type, size_t size, unsigned align) { return malloc(size); }
void *rte_zmalloc(const char *type, size_t size, unsigned align) { return malloc(size); }
void *rte_calloc(const char *type, size_t num, size_t size, unsigned align) { return calloc(num, size); }
void *rte_realloc(void *ptr, size_t size, unsigned int align) { return realloc(ptr, size); }
void *rte_realloc_socket(void *ptr, size_t size, unsigned int align, int socket) { return realloc(ptr, size); }
void *rte_malloc_socket(const char *type, size_t size, unsigned align, int socket) { return malloc(size); }
void *rte_zmalloc_socket(const char *type, size_t size, unsigned align, int socket) { return malloc(size); }
void *rte_calloc_socket(const char *type, size_t num, size_t size, unsigned align, int socket) { return calloc(num, size); }
int rte_memmory_ok(void *ptr) { return 0; }
int rte_malloc_validate(const void *ptr, size_t *size) { return 0; }
int rte_malloc_get_socket_stats(int socket, struct rte_malloc_socket_stats *socket_stats) { return 0; }
int rte_malloc_heap_memory_add(const char *heap_name, void *va_addr, size_t len, rte_iova_t iova_addrs[], unsigned int n_pages, size_t page_sz) { return 0; }
int rte_malloc_heap_memory_remove(const char *heap_name, void *va_addr, size_t len) { return 0; }
int rte_malloc_heap_memory_attach(const char *heap_name, void *va_addr, size_t len) { return 0; }
int rte_malloc_heap_memory_detach(const char *heap_name, void *va_addr, size_t len) { return 0; }
int rte_malloc_heap_create(const char *heap_name) { return 0; }
int rte_malloc_heap_destroy(const char *heap_name) { return 0; }
int rte_malloc_heap_get_socket(const char *name) { return 0; }
int rte_malloc_heap_socket_is_external(int socket_id) { return 0; }
void rte_malloc_dump_stats(FILE *f, const char *type) { }
void rte_malloc_dump_heaps(FILE *f) { }
rte_iova_t rte_malloc_virt2iova(const void *addr) { return 0; }