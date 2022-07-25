PERF_SRCS= perf_profiler.cc perf_profiler.h

all: perf-matmul-test perf-tlb-test

MATMUL_TEST_SRCS = $(PERF_SRCS) perf_matmul_test.cc matmul.cc matmul.h
perf-matmul-test: $(MATMUL_TEST_SRCS)
	g++ -Wall -I /usr/local/include -L /usr/local/lib $(MATMUL_TEST_SRCS) -o ../bin/perf_matmul_test -g -l:libpfm.a

TLB_TEST_SRCS = $(PERF_SRCS) perf_tlb_test.cc tlb_test.cc tlb_test.h
perf-tlb-test: $(TLB_TEST_SRCS)
	g++ -O2 -Wall -I /usr/local/include -L /usr/local/lib $(TLB_TEST_SRCS) -o ../bin/perf_tlb_test -g -l:libpfm.a
