#include "rte_memory.h"

int rte_mem_lock_page(const void *virt) { return 0; }
phys_addr_t rte_mem_virt2phy(const void *virt) { return 0; }
rte_iova_t rte_mem_virt2iova(const void *virt) { return 0; }
void *rte_mem_iova2virt(rte_iova_t iova) { return NULL; }
struct rte_memseg *rte_mem_virt2memseg(const void *virt, const struct rte_memseg_list *msl) { return NULL; }
struct rte_memseg_list *rte_mem_virt2memseg_list(const void *virt) { return NULL; }
int rte_memseg_walk(rte_memseg_walk_t func, void *arg) { return 0; }
int rte_memseg_contig_walk(rte_memseg_contig_walk_t func, void *arg) { return 0; }
int rte_memseg_list_walk(rte_memseg_list_walk_t func, void *arg) { return 0; }
int rte_memseg_walk_thread_unsafe(rte_memseg_walk_t func, void *arg) { return 0; }
int rte_memseg_contig_walk_thread_unsafe(rte_memseg_contig_walk_t func, void *arg) { return 0; }
int rte_memseg_list_walk_thread_unsafe(rte_memseg_list_walk_t func, void *arg) { return 0; }
int rte_memseg_get_fd(const struct rte_memseg *ms) { return 0; }
int rte_memseg_get_fd_thread_unsafe(const struct rte_memseg *ms) { return 0; }
int rte_memseg_get_fd_offset(const struct rte_memseg *ms, size_t *offset) { return 0; }
int rte_memseg_get_fd_offset_thread_unsafe(const struct rte_memseg *ms, size_t *offset) { return 0; }
int rte_extmem_register(void *va_addr, size_t len, rte_iova_t iova_addrs[], unsigned int n_pages, size_t page_sz) { return 0; }
int rte_extmem_unregister(void *va_addr, size_t len) { return 0; }
int rte_extmem_attach(void *va_addr, size_t len) { return 0; }
int rte_extmem_detach(void *va_addr, size_t len) { return 0; }
void rte_dump_physmem_layout(FILE *f) { }
uint64_t rte_eal_get_physmem_size(void) { return 0; }
unsigned rte_memory_get_nchannel(void) { return 0; }
unsigned rte_memory_get_nrank(void) { return 0; }
int rte_mem_check_dma_mask(uint8_t maskbits) { return 0; }
int rte_mem_check_dma_mask_thread_unsafe(uint8_t maskbits) { return 0; }
void rte_mem_set_dma_mask(uint8_t maskbits) { }
int rte_eal_using_phys_addrs(void) { return 0; }
int rte_mem_event_callback_register(const char *name, rte_mem_event_callback_t clb, void *arg) { return 0; }
int rte_mem_event_callback_unregister(const char *name, void *arg) { return 0; }
int rte_mem_alloc_validator_register(const char *name, rte_mem_alloc_validator_t clb, int socket_id, size_t limit) { return 0; }
int rte_mem_alloc_validator_unregister(const char *name, int socket_id) { return 0; } 