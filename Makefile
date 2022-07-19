SRCS=perf_profiler_wrapper_test.cc perf_profiler.cc perf_profiler.h matmul.cc matmul.h

perf-profiler-wrapper-test: $(SRCS)
	g++ -Wall -I /usr/local/include -L /usr/local/lib $(SRCS) -o perf_profiler_wrapper_test -g -l:libpfm.a
