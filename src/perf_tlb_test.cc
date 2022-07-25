#include "tlb_test.h"

void print_help() {
  printf("Help: run DTLB test\n"
    "usage: -m mapsize -s stride -n num_accesses -r randomize -c cache_level\n"
    "randomize: {0,1} cache_level: {L1,L2,LL}\n");
  exit(1);
}

int main(int argc, char *argv[]) {
  int c;
  unsigned long mapsize, stride;
  unsigned int num_accesses;
  bool randomize;
  std::string cache_level;

  if (argc < 5) {
    print_help();
  }

  while ((c = getopt(argc, argv, "m:s:n:r:c:")) != -1) {
    switch (c) {
      case 'm':
        mapsize = atoi(optarg);
        break;
      case 's':
        stride = atoi(optarg);
        break;
      case 'n':
        num_accesses = atoi(optarg);
        break;
      case 'r':
        randomize = atoi(optarg);
        break;
      case 'c':
        cache_level = std::string(optarg);
        break;
      case 'h':
        // fall through
      default:
        print_help();
        return -1;
    }
  }

  std::map<perf_type_config_t, std::string> counter_name_map;
  std::vector<std::string> event_list;

  if (cache_level == "L1") {
    event_list = {"L1-DCACHE-LOADS", "L1-DCACHE-LOAD-MISSES", "DTLB-LOADS", "DTLB-LOAD-MISSES"};
  } else if (cache_level == "L2") {
    event_list = {"L2_RQSTS:ALL_DEMAND_DATA_RD", "L2_RQSTS:DEMAND_DATA_RD_MISS", "DTLB-LOADS", "DTLB-LOAD-MISSES"};
  } else if (cache_level == "LL") {
    event_list = {"PERF_COUNT_HW_CACHE_LL:READ:ACCESS", "PERF_COUNT_HW_CACHE_LL:READ:MISS", "DTLB-LOADS", "DTLB-LOAD-MISSES"};
  } else {
    printf("invalid cache level: %s\n", cache_level.c_str());
    print_help();
  }

  // std::vector<std::string> event_list = {"L2_RQSTS:REFERENCES", "L2_RQSTS:MISS", "DTLB-LOADS", "DTLB-LOAD-MISSES"};
  // std::vector<std::string> event_list = {"L1-DCACHE-LOADS", "L1-DCACHE-LOAD-MISSES", "DTLB-LOADS", "DTLB-LOAD-MISSES"};
  // std::vector<std::string> event_list = {"L2_RQSTS:DEMAND_DATA_RD_HIT", "L2_RQSTS:DEMAND_DATA_RD_MISS", "DTLB-LOADS", "DTLB-LOAD-MISSES"};
  // std::vector<std::string> event_list = {"PERF_COUNT_HW_CACHE_LL:READ:ACCESS", "PERF_COUNT_HW_CACHE_LL:READ:MISS", "DTLB-LOADS", "DTLB-LOAD-MISSES"};
  
  // std::vector<std::string> event_list = {"L1-DCACHE-LOADS", "L1-DCACHE-LOAD-MISSES", "DTLB-LOADS", "DTLB-LOAD-MISSES"};
  // std::vector<std::string> event_list = {"L1-DCACHE-LOADS", "L1-DCACHE-LOAD-MISSES", "L1-DCACHE-PREFETCHES", "L1-DCACHE-PREFETCH-MISSES"};
  // std::vector<std::string> event_list = {"L1-DCACHE-LOADS", "L1-DCACHE-LOAD-MISSES", "L1-DCACHE-PREFETCHES"};
  // std::vector<std::string> event_list = {"PERF_COUNT_HW_CPU_CYCLES", "PERF_COUNT_HW_CACHE_L1D:PREFETCH:ACCESS"};
  // std::vector<std::string> event_list = {"L1-DCACHE-PREFETCHES", "L1-DCACHE-PREFETCH-MISSES"};

  perf_event_attr pea;
  PerfProfiler myperf(&counter_name_map, &event_list, NULL);
  

  int res = dtlb_test(mapsize, stride, num_accesses, randomize, &myperf);

  std::map<std::string, uint64_t> result_map = myperf.Read();

  for (const auto& kv : result_map) {
    printf("%s: %s\n", kv.first.c_str(), std::to_string(kv.second).c_str());
  }
  

  return 0;
}