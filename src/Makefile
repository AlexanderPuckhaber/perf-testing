PERFMON_INCLUDE_DIR = /usr/local/include
PERFMON_LIB_DIR = /usr/local/lib

include config.mk

PERF_SRCS= perf_profiler.cc perf_profiler.h

all: perf-matmul-test perf-tlb-test perf-matmul-runner

MATMUL_TEST_SRCS = $(PERF_SRCS) perf_matmul_test.cc matmul.cc matmul.h get_perf_configs.cc get_perf_configs.h
perf-matmul-test: $(MATMUL_TEST_SRCS)
	g++ -Wall -I /usr/local/include -L /usr/local/lib $(MATMUL_TEST_SRCS) -o ../bin/perf_matmul_test -g -l:libpfm.a

MATMUL_RUNNER_SRCS = $(PERF_SRCS) perf_matmul_runner.cc matmul.cc matmul.h get_perf_configs.cc get_perf_configs.h
perf-matmul-runner: $(MATMUL_RUNNER_SRCS)
	g++ -Wall -I $(PERFMON_INCLUDE_DIR) -L $(PERFMON_LIB_DIR) $(MATMUL_RUNNER_SRCS) -o ../bin/perf_matmul_runner -g -l:libpfm.a

MATMUL_TLB_RUNNER_SRCS = $(PERF_SRCS) perf_matmul_tlb_runner.cc matmul.cc matmul.h get_perf_configs.cc get_perf_configs.h mappings.h mappings.cc
perf-matmul-tlb-runner: $(MATMUL_TLB_RUNNER_SRCS)
	g++ -Wall -I $(PERFMON_INCLUDE_DIR) -L $(PERFMON_LIB_DIR) $(MATMUL_TLB_RUNNER_SRCS) -o ../bin/perf_matmul_tlb_runner -g -l:libpfm.a

MATMUL_SPARSE_TLB_RUNNER_SRCS = $(PERF_SRCS) perf_matmul_sparse_tlb_runner.cc matmul.cc matmul.h get_perf_configs.cc get_perf_configs.h mappings.h mappings.cc
perf-matmul-sparse-tlb-runner: $(MATMUL_TLB_SPARSE_RUNNER_SRCS)
	g++ -Wall -I $(PERFMON_INCLUDE_DIR) -L $(PERFMON_LIB_DIR) $(MATMUL_SPARSE_TLB_RUNNER_SRCS) -o ../bin/perf_matmul_sparse_tlb_runner -g -l:libpfm.a

TLB_TEST_SRCS = $(PERF_SRCS) perf_tlb_test.cc tlb_test.cc tlb_test.h
perf-tlb-test: $(TLB_TEST_SRCS)
	g++ -O2 -Wall -I /usr/local/include -L /usr/local/lib $(TLB_TEST_SRCS) -o ../bin/perf_tlb_test -g -l:libpfm.a

MAP_TEST_SRCS = $(PERF_SRCS) map_test.cc get_perf_configs.cc get_perf_configs.h
perf-map-test: $(MAP_TEST_SRCS)
	g++ -O2 -Wall -I /usr/local/include -L /usr/local/lib $(MAP_TEST_SRCS) -o ../bin/perf_map_test -g -l:libpfm.a
