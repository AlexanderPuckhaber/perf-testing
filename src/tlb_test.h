#include <inttypes.h>
#include <sys/mman.h>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <errno.h>

#include "perf_profiler.h"

int dtlb_test(unsigned long size, unsigned long stride, unsigned int num_accesses, bool randomize, PerfProfiler *myperf);