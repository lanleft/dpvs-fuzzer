cmake_minimum_required(VERSION 3.13)
project(dpvs-fuzzer)

# Set AFL++ compiler
set(CMAKE_CXX_COMPILER "/home/lab/Desktop/side-projects/linux-setup/dpvs-fuzzer/third_party/AFLplusplus/afl-clang-fast++")
set(CMAKE_C_COMPILER "/home/lab/Desktop/side-projects/linux-setup/dpvs-fuzzer/third_party/AFLplusplus/afl-clang-fast")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(DPDK_LIB_PATH "/home/lab/Desktop/side-projects/linux-setup/dpvs-fuzzer/third_party/dpdk-24.11/dpdklib")

# Force coloful output
add_compile_options(-fcolor-diagnostics)

# ==================== DPDK DEPENDENCIES ====================

# DPDK libraries define
set(DPDK_LIBS
    -L${DPDK_LIB_PATH}/lib/x86_64-linux-gnu
    -Wl,--as-needed
    -lrte_node -lrte_graph -lrte_pipeline -lrte_table -lrte_pdump
    -lrte_port -lrte_fib -lrte_pdcp -lrte_ipsec -lrte_vhost -lrte_stack
    -lrte_security -lrte_sched -lrte_reorder -lrte_rib -lrte_mldev
    -lrte_regexdev -lrte_rawdev -lrte_power -lrte_pcapng -lrte_member
    -lrte_lpm -lrte_latencystats -lrte_jobstats -lrte_ip_frag -lrte_gso
    -lrte_gro -lrte_gpudev -lrte_dispatcher -lrte_eventdev -lrte_efd
    -lrte_dmadev -lrte_distributor -lrte_cryptodev -lrte_compressdev
    -lrte_cfgfile -lrte_bpf -lrte_bitratestats -lrte_bbdev -lrte_acl
    -lrte_timer -lrte_hash -lrte_metrics -lrte_cmdline -lrte_pci
    -lrte_ethdev -lrte_meter -lrte_net -lrte_mbuf -lrte_mempool
    -lrte_rcu -lrte_ring -lrte_telemetry -lrte_argparse
    -lrte_kvargs -lrte_log -lrte_eal
)

set(DPDK_EAL_COMBINED_COMPONENTS
    -l:librte_eal.a
    -l:librte_bus_pci.a
    -l:librte_common_cnxk.a
)

# net
set(DPDK_NET_COMPONENTS 
  -l:librte_net_atlantic.a  -l:librte_net_avp.a -l:librte_net_axgbe.a -l:librte_net_bnx2x.a -l:librte_net_bnxt.a -l:librte_net_e1000.a -l:librte_net_ena.a -l:librte_net_enic.a -l:librte_net_failsafe.a -l:librte_net_fm10k.a -l:librte_net_gve.a -l:librte_net_hinic.a -l:librte_net_hns3.a -l:librte_net_i40e.a -l:librte_net_iavf.a -l:librte_net_ice.a -l:librte_net_idpf.a -l:librte_net_igc.a -l:librte_net_ionic.a -l:librte_net_ixgbe.a -l:librte_net_memif.a -l:librte_net_mlx4.a -l:librte_net_mlx5.a -l:librte_net_netvsc.a -l:librte_net_nfp.a -l:librte_net_ngbe.a -l:librte_net_ntnic.a -l:librte_net_null.a -l:librte_net_octeontx.a -l:librte_net_octeon_ep.a -l:librte_net_pfe.a -l:librte_net_qede.a -l:librte_net_r8169.a -l:librte_net_ring.a -l:librte_net_sfc.a -l:librte_net_softnic.a -l:librte_net_tap.a -l:librte_net_thunderx.a -l:librte_net_txgbe.a -l:librte_net_vdev_netvsc.a -l:librte_net_vhost.a -l:librte_net_virtio.a -l:librte_net_vmxnet3.a -l:librte_net_zxdh.a
)

set(DPDK_RAW_COMPONENTS
  -l:librte_raw_ntb.a -l:librte_raw_skeleton.a -l:librte_crypto_bcmfs.a -l:librte_crypto_caam_jr.a -l:librte_crypto_ccp.a -l:librte_crypto_cnxk.a -l:librte_crypto_dpaa_sec.a -l:librte_crypto_dpaa2_sec.a -l:librte_crypto_ionic.a -l:librte_crypto_mlx5.a -l:librte_crypto_nitrox.a -l:librte_crypto_null.a -l:librte_crypto_octeontx.a -l:librte_crypto_openssl.a -l:librte_crypto_scheduler.a -l:librte_crypto_virtio.a -l:librte_compress_mlx5.a -l:librte_compress_nitrox.a -l:librte_compress_octeontx.a -l:librte_compress_zlib.a -l:librte_regex_mlx5.a -l:librte_regex_cn9k.a -l:librte_ml_cnxk.a -l:librte_vdpa_ifc.a -l:librte_vdpa_mlx5.a -l:librte_vdpa_nfp.a -l:librte_vdpa_sfc.a -l:librte_event_cnxk.a -l:librte_event_dlb2.a -l:librte_event_dpaa.a -l:librte_event_dpaa2.a -l:librte_event_dsw.a -l:librte_event_opdl.a -l:librte_event_skeleton.a -l:librte_event_sw.a -l:librte_event_octeontx.a -l:librte_baseband_acc.a -l:librte_baseband_fpga_5gnr_fec.a -l:librte_baseband_fpga_lte_fec.a -l:librte_baseband_la12xx.a -l:librte_baseband_null.a -l:librte_baseband_turbo_sw.a -l:librte_power_acpi.a -l:librte_power_amd_pstate.a -l:librte_power_cppc.a -l:librte_power_intel_pstate.a -l:librte_power_intel_uncore.a -l:librte_power_kvm_vm.a -l:librte_node.a -l:librte_graph.a -l:librte_pipeline.a -l:librte_table.a -l:librte_pdump.a -l:librte_port.a -l:librte_fib.a -l:librte_pdcp.a -l:librte_ipsec.a -l:librte_vhost.a -l:librte_stack.a -l:librte_security.a -l:librte_sched.a -l:librte_reorder.a -l:librte_rib.a 
)

# dev
set(DPDK_DEV_COMPONENTS
  -l:librte_mldev.a -l:librte_regexdev.a -l:librte_rawdev.a -l:librte_power.a -l:librte_pcapng.a -l:librte_member.a -l:librte_lpm.a -l:librte_latencystats.a -l:librte_jobstats.a -l:librte_ip_frag.a -l:librte_gso.a -l:librte_gro.a -l:librte_gpudev.a -l:librte_dispatcher.a -l:librte_eventdev.a -l:librte_efd.a -l:librte_dmadev.a -l:librte_distributor.a -l:librte_cryptodev.a -l:librte_compressdev.a -l:librte_bbdev.a -l:librte_ethdev.a
)

# bus
set(DPDK_BUS_COMPONENTS
  -l:librte_bus_auxiliary.a -l:librte_bus_cdx.a -l:librte_bus_dpaa.a -l:librte_bus_fslmc.a -l:librte_bus_ifpga.a  -l:librte_bus_platform.a -l:librte_bus_uacce.a -l:librte_bus_vdev.a -l:librte_bus_vmbus.a
)

# common 
set(DPDK_COMMON_COMPONENTS
  -l:librte_common_cpt.a -l:librte_common_dpaax.a -l:librte_common_iavf.a -l:librte_common_idpf.a -l:librte_common_ionic.a -l:librte_common_octeontx.a   -l:librte_common_mlx5.a -l:librte_common_nfp.a -l:librte_common_nitrox.a -l:librte_common_qat.a -l:librte_common_sfc_efx.a
)

# dma
set(DPDK_DMA_COMPONENTS
  -l:librte_dma_cnxk.a -l:librte_dma_dpaa.a -l:librte_dma_dpaa2.a -l:librte_dma_hisilicon.a -l:librte_dma_idxd.a -l:librte_dma_ioat.a -l:librte_dma_odm.a -l:librte_dma_skeleton.a
)

# mempool
set(DPDK_MEMPOOL_COMPONENTS
  -l:librte_mempool_bucket.a -l:librte_mempool_cnxk.a -l:librte_mempool_dpaa.a -l:librte_mempool_dpaa2.a -l:librte_mempool_octeontx.a -l:librte_mempool_ring.a -l:librte_mempool_stack.a
)

# net2
set(DPDK_NET2_COMPONENTS
  -l:librte_net_af_packet.a -l:librte_net_ark.a     -l:librte_net_bond.a -l:librte_net_cnxk.a -l:librte_net_cpfl.a -l:librte_net_cxgbe.a -l:librte_net_dpaa.a -l:librte_net_dpaa2.a   -l:librte_net_enetc.a -l:librte_net_enetfec.a
)

# raw
set(DPDK_RAW_COMPONENTS
  -l:librte_raw_cnxk_bphy.a -l:librte_raw_cnxk_gpio.a -l:librte_raw_cnxk_rvu_lf.a -l:librte_raw_dpaa2_cmdif.a -l:librte_raw_gdtc.a
)

# others
set(DPDK_OTHERS_COMPONENTS
  -l:librte_metrics.a -l:librte_cfgfile.a -l:librte_bpf.a -l:librte_bitratestats.a  -l:librte_acl.a -l:librte_timer.a -l:librte_hash.a  -l:librte_cmdline.a -l:librte_pci.a  -l:librte_meter.a -l:librte_net.a -l:librte_mbuf.a -l:librte_mempool.a -l:librte_rcu.a -l:librte_ring.a  -l:librte_telemetry.a -l:librte_argparse.a -l:librte_kvargs.a -l:librte_log.a
)

set(DPDK_COMPONENTS
  -Wl,--whole-archive
  -Wl,--no-whole-archive -Wl,--export-dynamic -latomic
  ${DPDK_EAL_COMPONENTS}
  ${DPDK_NET_COMPONENTS}
  ${DPDK_RAW_COMPONENTS}
  ${DPDK_DEV_COMPONENTS}
  ${DPDK_BUS_COMPONENTS}
  ${DPDK_COMMON_COMPONENTS}
  ${DPDK_DMA_COMPONENTS}
  ${DPDK_MEMPOOL_COMPONENTS}
  ${DPDK_NET2_COMPONENTS}
  ${DPDK_RAW_COMPONENTS}
  ${DPDK_OTHERS_COMPONENTS}
)

set(DPDK_INCLUDE
    third_party/dpdk-24.11/dpdklib/include
)

# =================== AFL++ FLAGS ==================

# DATE_STRING := $(shell date +%Y.%m.%d.%H:%M:%S)
set(DATE_STRING "2025.05.15.17:00:00")

# AFL++ flags , debug mode
set(AFL_CXX_FLAGS
    -fsanitize=fuzzer,address
    -O1
    -g
    -DDPVS_BUILD_DATE="${DATE_STRING}"
)

# AFL++ flags , debug mode
set(AFL_CXX_FLAGS_NON_FUZZ
    -fsanitize=address
    -O1
    -g
    -DDPVS_BUILD_DATE="${DATE_STRING}"
)
# debug mode
set(AFL_LD_FLAGS
    -fsanitize=fuzzer,address
    -g
    -O1
    -lnuma
    -lz 
)

set(AFL_LD_FLAGS_NON_FUZZ
    -fsanitize=address
    -g
    -O1
    -lnuma
    -lz 
)

# ==================== DPVS DEPENDENCIES ====================

# building all *.c files in dpvs

# DPVS libraries define
set(DPVS_DEFINES
    -DDPVS_MAX_LCORE=4
    -DDPVS_MAX_SOCKET=2
    -D__clang__
)

set(DPVS_INCLUDE
  third_party/dpvs/include/
  third_party/dpvs/tools/keepalived/keepalived/include/
  third_party/dpvs/tools/
)

set(DPVS_HEADERS
  fuzz/fakes/dpvs-config.h
  third_party/dpvs/src/ipvs/libconhash/util_rbtree.h
  third_party/dpvs/src/ipvs/libconhash/conhash_inter.h
  third_party/dpvs/src/ipvs/libconhash/conhash.h
  third_party/dpvs/src/ipvs/libconhash/configure.h
  third_party/dpvs/src/ipvs/libconhash/md5.h
)

set(DPVS_SOURCES
  third_party/dpvs/src/mempool.c
  third_party/dpvs/src/linux_if.c
  third_party/dpvs/src/parser.c
  third_party/dpvs/src/iftraf.c
  third_party/dpvs/src/log.c
  third_party/dpvs/src/tc/sch_pfifo_fast.c
  third_party/dpvs/src/tc/sch_fifo.c
  third_party/dpvs/src/tc/cls_ipset.c
  third_party/dpvs/src/tc/tc.c
  third_party/dpvs/src/tc/cls.c
  third_party/dpvs/src/tc/cls_match.c
  third_party/dpvs/src/tc/sch_shm.c
  third_party/dpvs/src/tc/tc_ctrl.c
  third_party/dpvs/src/tc/sch_generic.c
  third_party/dpvs/src/tc/sch_tbf.c
  third_party/dpvs/src/neigh.c
  third_party/dpvs/src/debug.c
  third_party/dpvs/src/ip_gre.c
  third_party/dpvs/src/scheduler.c
  third_party/dpvs/src/sa_pool.c
  third_party/dpvs/src/global_conf.c
  third_party/dpvs/src/cfgfile.c
  third_party/dpvs/src/common.c
  third_party/dpvs/src/ipv4.c
  third_party/dpvs/src/eal_mem.c
  third_party/dpvs/src/sys_time.c
  third_party/dpvs/src/inetaddr.c
  third_party/dpvs/src/netif.c
  third_party/dpvs/src/inet.c
  third_party/dpvs/src/ipvs/ip_vs_sched.c
  third_party/dpvs/src/ipvs/ip_vs_proto_tcp.c
  third_party/dpvs/src/ipvs/ip_vs_dest.c
  third_party/dpvs/src/ipvs/ip_vs_conhash.c
  third_party/dpvs/src/ipvs/ip_vs_whtlst.c
  third_party/dpvs/src/ipvs/kcompat.c
  third_party/dpvs/src/ipvs/libconhash/conhash_util.c
  third_party/dpvs/src/ipvs/libconhash/util_rbtree.c
  third_party/dpvs/src/ipvs/libconhash/md5.c
  third_party/dpvs/src/ipvs/libconhash/conhash.c
  third_party/dpvs/src/ipvs/libconhash/conhash_inter.c
  third_party/dpvs/src/ipvs/siphash.c
  third_party/dpvs/src/ipvs/ip_vs_laddr.c
  third_party/dpvs/src/ipvs/ip_vs_nat64.c
  third_party/dpvs/src/ipvs/ip_vs_redirect.c
  third_party/dpvs/src/ipvs/ip_vs_stats.c
  third_party/dpvs/src/ipvs/ip_vs_wrr.c
  third_party/dpvs/src/ipvs/ip_vs_proxy_proto.c
  third_party/dpvs/src/ipvs/ip_vs_xmit.c
  third_party/dpvs/src/ipvs/ip_vs_core.c
  third_party/dpvs/src/ipvs/ip_vs_proto_icmp.c
  third_party/dpvs/src/ipvs/ip_vs_blklst.c
  third_party/dpvs/src/ipvs/ip_vs_service.c
  third_party/dpvs/src/ipvs/ip_vs_proto_udp.c
  third_party/dpvs/src/ipvs/ip_vs_wlc.c
  third_party/dpvs/src/ipvs/ip_vs_quic.c
  third_party/dpvs/src/ipvs/ip_vs_mh.c
  third_party/dpvs/src/ipvs/ip_vs_fo.c
  third_party/dpvs/src/ipvs/ip_vs_proto.c
  third_party/dpvs/src/ipvs/ip_vs_proto_sctp.c
  third_party/dpvs/src/ipvs/ip_vs_conn.c
  third_party/dpvs/src/ipvs/ip_vs_synproxy.c
  third_party/dpvs/src/ipvs/ip_vs_rr.c
  third_party/dpvs/src/global_data.c
  third_party/dpvs/src/netif_flow.c
  third_party/dpvs/src/ipset/ipset_hash_ipport.c
  third_party/dpvs/src/ipset/ipset_bitmap_port.c
  third_party/dpvs/src/ipset/ipset_bitmap_ipmac.c
  third_party/dpvs/src/ipset/ipset_ctrl.c
  third_party/dpvs/src/ipset/ipset_hash_netport.c
  third_party/dpvs/src/ipset/ipset_hash_ipportip.c
  third_party/dpvs/src/ipset/ipset_bitmap_ip.c
  third_party/dpvs/src/ipset/ipset_hash_netportnetport.c
  third_party/dpvs/src/ipset/ipset_hash.c
  third_party/dpvs/src/ipset/ipset_hash_netportiface.c
  third_party/dpvs/src/ipset/ipset_core.c
  third_party/dpvs/src/ipset/ipset_bitmap.c
  third_party/dpvs/src/ipset/ipset_hash_net.c
  third_party/dpvs/src/ipset/ipset_hash_ipportnet.c
  third_party/dpvs/src/ipset/ipset_hash_netportnet.c
  third_party/dpvs/src/ipset/ipset_hash_ip.c
  third_party/dpvs/src/ipset/pfxlen.c
  third_party/dpvs/src/ipip.c
  third_party/dpvs/src/route.c
  third_party/dpvs/src/vlan.c
  third_party/dpvs/src/icmp.c
  third_party/dpvs/src/lldp.c
  third_party/dpvs/src/pdump.c
  third_party/dpvs/src/netif_addr.c
  third_party/dpvs/src/ip_tunnel.c
  third_party/dpvs/src/ctrl.c
  third_party/dpvs/src/mbuf.c
  third_party/dpvs/src/ipv6/route6.c
  third_party/dpvs/src/ipv6/route6_hlist.c
  third_party/dpvs/src/ipv6/ipv6_ctrl.c
  third_party/dpvs/src/ipv6/route6_lpm.c
  third_party/dpvs/src/ipv6/icmp6.c
  third_party/dpvs/src/ipv6/ndisc.c
  third_party/dpvs/src/ipv6/ipv6.c
  third_party/dpvs/src/ipv6/ipv6_exthdrs.c
  third_party/dpvs/src/ipv4_frag.c
  third_party/dpvs/src/timer.c
  third_party/dpvs/src/pidfile.c
  third_party/dpvs/tools/keepalived/keepalived/check/sockopt.c
  third_party/dpvs/tools/dpip/utils.c
  third_party/dpvs/tools/dpip/dpip.c
  third_party/dpvs/tools/dpip/route.c
)



# ==================== BUILDING FUZZER ====================


find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# libprotobuf-mutator
add_library(
  protobuf-mutator STATIC
  third_party/libprotobuf-mutator/src/binary_format.cc
  third_party/libprotobuf-mutator/src/mutator.cc
  third_party/libprotobuf-mutator/src/text_format.cc
  third_party/libprotobuf-mutator/src/utf8_fix.cc
  third_party/libprotobuf-mutator/src/libfuzzer/libfuzzer_macro.cc
  third_party/libprotobuf-mutator/src/libfuzzer/libfuzzer_mutator.cc
  third_party/libprotobuf-mutator/src/mutator.h
  ${DPVS_HEADERS}
)

target_include_directories(
  protobuf-mutator PRIVATE third_party/libprotobuf-mutator
)
target_compile_options(protobuf-mutator PRIVATE -std=c++14)


target_link_libraries(protobuf-mutator protobuf)

# Generate protobuf files
protobuf_generate_cpp(MBUF_PROTO_SRCS MBUF_PROTO_HDRS fuzz/mbuf_mutator.proto)

# Add fuzz target
add_executable(
  dpvs-fuzzer fuzz/dpvs-fuzzer.cc ${MBUF_PROTO_SRCS} ${MBUF_PROTO_HDRS} ${DPVS_SOURCES}
)

target_include_directories(dpvs-fuzzer PRIVATE 
  third_party/libprotobuf-mutator
  ${DPVS_INCLUDE}
  ${DPDK_INCLUDE}
  fuzz/fakes
)
target_compile_options(dpvs-fuzzer PRIVATE ${AFL_CXX_FLAGS} ${DPVS_DEFINES} ${DPDK_LIBS}
  -mssse3
  -ferror-limit=3
  -Wall -Wstrict-prototypes -Wmissing-prototypes -mcmodel=medium
  -mcrc32
  -DLLVM_FUZZER=1
  -D__DPVS__
  -DDPVS_VERSION="1.10"
)

target_link_libraries(dpvs-fuzzer ${AFL_LD_FLAGS}
            -lprotobuf-mutator 
            -lprotobuf-mutator-libfuzzer
            ${Protobuf_LIBRARIES}
            ${DPDK_LIBS}
            ${DPDK_COMPONENTS}
            -lz
            -lmlx5
            -libverbs
            -lcrypto
            -lmlx4
            -lelf
)

# ==================== BUILDING BATCH TEST ====================
add_executable(
  dpvs-fuzzer-batch fuzz/dpvs-fuzzer.cc ${MBUF_PROTO_SRCS} ${MBUF_PROTO_HDRS} ${DPVS_SOURCES}
)

target_include_directories(dpvs-fuzzer-batch PRIVATE 
  third_party/libprotobuf-mutator
  ${DPVS_INCLUDE}
  ${DPDK_INCLUDE}
  fuzz/fakes)

target_compile_options(dpvs-fuzzer-batch PRIVATE ${AFL_CXX_FLAGS_NON_FUZZ} ${DPVS_DEFINES} ${DPDK_LIBS}
  -mssse3
  -ferror-limit=3
  -Wall -Wstrict-prototypes -Wmissing-prototypes -mcmodel=medium
  -mcrc32
  -DLLVM_FUZZER=1
  -D__DPVS__
  -DDPVS_VERSION="1.10"
  -DLLVM_BATCH_TEST=1
)

target_link_libraries(dpvs-fuzzer-batch ${AFL_LD_FLAGS_NON_FUZZ}
  -lprotobuf-mutator  
  -lprotobuf-mutator-libfuzzer  
  ${Protobuf_LIBRARIES}
  ${DPDK_LIBS}
  ${DPDK_COMPONENTS}
  -lz
  -lmlx5
  -libverbs
  -lcrypto
  -lmlx4
  -lelf
)
# ==================== BUILDING COVERAGE REPORT ====================
# Build coverage report
add_executable(
  dpvs-fuzzer-cov fuzz/dpvs-fuzzer.cc ${MBUF_PROTO_SRCS} ${MBUF_PROTO_HDRS} ${DPVS_SOURCES}
)

target_include_directories(dpvs-fuzzer-cov PRIVATE 
  third_party/libprotobuf-mutator
  ${DPVS_INCLUDE}
  ${DPDK_INCLUDE}
  fuzz/fakes)

# Add clang coverage flags
target_compile_options(dpvs-fuzzer-cov PRIVATE ${AFL_CXX_FLAGS} ${DPVS_DEFINES} ${DPDK_LIBS}
  -mssse3
  -ferror-limit=3
  -Wall -Wstrict-prototypes -Wmissing-prototypes -mcmodel=medium
  -mcrc32
  -DLLVM_FUZZER=1
  -D__DPVS__
  -DDPVS_VERSION="1.10"
  -fprofile-instr-generate
  -fcoverage-mapping
)

target_link_libraries(dpvs-fuzzer-cov ${AFL_LD_FLAGS}
  -lprotobuf-mutator  
  -lprotobuf-mutator-libfuzzer  
  ${Protobuf_LIBRARIES}
  ${DPDK_LIBS}
  ${DPDK_COMPONENTS}
  -lz
  -lmlx5
  -libverbs
  -lcrypto
  -lmlx4
  -lelf
  -fprofile-instr-generate
  -fcoverage-mapping
)

# # Add coverage report target
# add_custom_command(
#   TARGET dpvs-fuzzer-cov POST_BUILD
#   COMMAND ${CMAKE_OBJCOPY} --only-keep-debug ${CMAKE_BINARY_DIR}/dpvs-fuzzer-cov ${CMAKE_BINARY_DIR}/dpvs-fuzzer-cov.dbg
#   COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=${CMAKE_BINARY_DIR}/dpvs-fuzzer-cov.dbg ${CMAKE_BINARY_DIR}/dpvs-fuzzer-cov
# )


