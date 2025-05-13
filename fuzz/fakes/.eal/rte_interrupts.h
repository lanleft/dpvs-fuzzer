// faking rte interrupt functions


// interrupt handle
struct rte_intr_handle;

#ifdef __cplusplus
extern "C" {
#endif

int rte_intr_efds_index_get(const struct rte_intr_handle *intr_handle, int index);

int rte_intr_efds_index_set(struct rte_intr_handle *intr_handle, int index, int fd);

#ifdef __cplusplus
}
#endif
// int rte_intr_efds_index_set(struct rte_intr_handle *intr_handle, int index, int fd);
