PERF_SRCS= perf_profiler.cc perf_profiler.h
PERFMON_INCLUDE_DIR = /home/apuckhaber3/git/libpfm4/local_install/include
PERFMON_LIB_DIR = /home/apuckhaber3/git/libpfm4/local_install/lib

all: perf-matmul-test perf-tlb-test perf-matmul-runner

MATMUL_TEST_SRCS = $(PERF_SRCS) perf_matmul_test.cc matmul.cc matmul.h
perf-matmul-test: $(MATMUL_TEST_SRCS)
	g++ -Wall -I /usr/local/include -L /usr/local/lib $(MATMUL_TEST_SRCS) -o ../bin/perf_matmul_test -g -l:libpfm.a

MATMUL_RUNNER_SRCS = $(PERF_SRCS) perf_matmul_runner.cc matmul.cc matmul.h get_perf_configs.cc get_perf_configs.h
perf-matmul-runner: $(MATMUL_RUNNER_SRCS)
	g++ -Wall -I $(PERFMON_INCLUDE_DIR) -L $(PERFMON_LIB_DIR) $(MATMUL_RUNNER_SRCS) -o ../bin/perf_matmul_runner -g -l:libpfm.a

TLB_TEST_SRCS = $(PERF_SRCS) perf_tlb_test.cc tlb_test.cc tlb_test.h
perf-tlb-test: $(TLB_TEST_SRCS)
	g++ -O2 -Wall -I /usr/local/include -L /usr/local/lib $(TLB_TEST_SRCS) -o ../bin/perf_tlb_test -g -l:libpfm.a
