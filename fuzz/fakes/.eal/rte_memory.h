#ifndef _RTE_MEMORY_H_
#define _RTE_MEMORY_H_

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RTE_PGSIZE_4K   (1ULL << 12)
#define RTE_PGSIZE_64K  (1ULL << 16)
#define RTE_PGSIZE_256K (1ULL << 18)
#define RTE_PGSIZE_2M   (1ULL << 21)
#define RTE_PGSIZE_16M  (1ULL << 24)
#define RTE_PGSIZE_256M (1ULL << 28)
#define RTE_PGSIZE_512M (1ULL << 29)
#define RTE_PGSIZE_1G   (1ULL << 30)
#define RTE_PGSIZE_4G   (1ULL << 32)
#define RTE_PGSIZE_16G  (1ULL << 34)
#define SOCKET_ID_ANY -1
#define RTE_MEM_EVENT_CALLBACK_NAME_LEN 64
#define RTE_MEM_ALLOC_VALIDATOR_NAME_LEN 64

struct rte_memseg { void *addr; };
struct rte_memseg_list { void *base_va; };
typedef uint64_t rte_iova_t;
typedef uint64_t phys_addr_t;

enum rte_mem_event {
    RTE_MEM_EVENT_ALLOC = 0,
    RTE_MEM_EVENT_FREE,
};

typedef int (*rte_memseg_walk_t)(const struct rte_memseg_list *msl, const struct rte_memseg *ms, void *arg);
typedef int (*rte_memseg_contig_walk_t)(const struct rte_memseg_list *msl, const struct rte_memseg *ms, size_t len, void *arg);
typedef int (*rte_memseg_list_walk_t)(const struct rte_memseg_list *msl, void *arg);
typedef void (*rte_mem_event_callback_t)(enum rte_mem_event event_type, const void *addr, size_t len, void *arg);
typedef int (*rte_mem_alloc_validator_t)(int socket_id, size_t cur_limit, size_t new_len);

int rte_mem_lock_page(const void *virt);
phys_addr_t rte_mem_virt2phy(const void *virt);
rte_iova_t rte_mem_virt2iova(const void *virt);
void *rte_mem_iova2virt(rte_iova_t iova);
struct rte_memseg *rte_mem_virt2memseg(const void *virt, const struct rte_memseg_list *msl);
struct rte_memseg_list *rte_mem_virt2memseg_list(const void *virt);
int rte_memseg_walk(rte_memseg_walk_t func, void *arg);
int rte_memseg_contig_walk(rte_memseg_contig_walk_t func, void *arg);
int rte_memseg_list_walk(rte_memseg_list_walk_t func, void *arg);
int rte_memseg_walk_thread_unsafe(rte_memseg_walk_t func, void *arg);
int rte_memseg_contig_walk_thread_unsafe(rte_memseg_contig_walk_t func, void *arg);
int rte_memseg_list_walk_thread_unsafe(rte_memseg_list_walk_t func, void *arg);
int rte_memseg_get_fd(const struct rte_memseg *ms);
int rte_memseg_get_fd_thread_unsafe(const struct rte_memseg *ms);
int rte_memseg_get_fd_offset(const struct rte_memseg *ms, size_t *offset);
int rte_memseg_get_fd_offset_thread_unsafe(const struct rte_memseg *ms, size_t *offset);
int rte_extmem_register(void *va_addr, size_t len, rte_iova_t iova_addrs[], unsigned int n_pages, size_t page_sz);
int rte_extmem_unregister(void *va_addr, size_t len);
int rte_extmem_attach(void *va_addr, size_t len);
int rte_extmem_detach(void *va_addr, size_t len);
void rte_dump_physmem_layout(FILE *f);
uint64_t rte_eal_get_physmem_size(void);
unsigned rte_memory_get_nchannel(void);
unsigned rte_memory_get_nrank(void);
int rte_mem_check_dma_mask(uint8_t maskbits);
int rte_mem_check_dma_mask_thread_unsafe(uint8_t maskbits);
void rte_mem_set_dma_mask(uint8_t maskbits);
int rte_eal_using_phys_addrs(void);
int rte_mem_event_callback_register(const char *name, rte_mem_event_callback_t clb, void *arg);
int rte_mem_event_callback_unregister(const char *name, void *arg);
int rte_mem_alloc_validator_register(const char *name, rte_mem_alloc_validator_t clb, int socket_id, size_t limit);
int rte_mem_alloc_validator_unregister(const char *name, int socket_id);

#ifdef __cplusplus
}
#endif

#endif /* _RTE_MEMORY_H_ */ 