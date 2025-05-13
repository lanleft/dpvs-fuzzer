#ifndef _RTE_MALLOC_H_
#define _RTE_MALLOC_H_

#include <stdio.h>
#include <stddef.h>
#include "rte_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

struct rte_malloc_socket_stats {
    size_t heap_totalsz_bytes;
    size_t heap_freesz_bytes;
    size_t greatest_free_size;
    unsigned free_count;
    unsigned alloc_count;
    size_t heap_allocsz_bytes;
};

void rte_free(void *ptr);
void *rte_malloc(const char *type, size_t size, unsigned align);
void *rte_zmalloc(const char *type, size_t size, unsigned align);
void *rte_calloc(const char *type, size_t num, size_t size, unsigned align);
void *rte_realloc(void *ptr, size_t size, unsigned int align);
void *rte_realloc_socket(void *ptr, size_t size, unsigned int align, int socket);
void *rte_malloc_socket(const char *type, size_t size, unsigned align, int socket);
void *rte_zmalloc_socket(const char *type, size_t size, unsigned align, int socket);
void *rte_calloc_socket(const char *type, size_t num, size_t size, unsigned align, int socket);
int rte_memmory_ok(void *ptr);
int rte_malloc_validate(const void *ptr, size_t *size);
int rte_malloc_get_socket_stats(int socket, struct rte_malloc_socket_stats *socket_stats);
int rte_malloc_heap_memory_add(const char *heap_name, void *va_addr, size_t len, rte_iova_t iova_addrs[], unsigned int n_pages, size_t page_sz);
int rte_malloc_heap_memory_remove(const char *heap_name, void *va_addr, size_t len);
int rte_malloc_heap_memory_attach(const char *heap_name, void *va_addr, size_t len);
int rte_malloc_heap_memory_detach(const char *heap_name, void *va_addr, size_t len);
int rte_malloc_heap_create(const char *heap_name);
int rte_malloc_heap_destroy(const char *heap_name);
int rte_malloc_heap_get_socket(const char *name);
int rte_malloc_heap_socket_is_external(int socket_id);
void rte_malloc_dump_stats(FILE *f, const char *type);
void rte_malloc_dump_heaps(FILE *f);
rte_iova_t rte_malloc_virt2iova(const void *addr);

#ifdef __cplusplus
}
#endif

#endif /* _RTE_MALLOC_H_ */