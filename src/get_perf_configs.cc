#include "get_perf_configs.h"

std::vector<std::string> common_sw_counters = {"perf::PERF_COUNT_SW_PAGE_FAULTS", "perf::PERF_COUNT_SW_CONTEXT_SWITCHES"};

PerfProfiler CFG_L1_LL() {
  std::map<perf_type_config_t, std::string> counter_name_map;
  std::vector<std::string> event_list;

  event_list = {"L1-DCACHE-LOADS", "L1-DCACHE-LOAD-MISSES", "PERF_COUNT_HW_CACHE_LL:READ:ACCESS", "PERF_COUNT_HW_CACHE_LL:READ:MISS"};
  event_list.insert(std::end(event_list), std::begin(common_sw_counters), std::end(common_sw_counters));

  PerfProfiler myperf(&counter_name_map, &event_list, NULL);

  return myperf;
}

PerfProfiler CFG_BRANCHES() {
  std::map<perf_type_config_t, std::string> counter_name_map;
  std::vector<std::string> event_list;

  event_list = {"perf::PERF_COUNT_HW_INSTRUCTIONS", "perf::PERF_COUNT_HW_BRANCH_INSTRUCTIONS", "perf::PERF_COUNT_HW_BRANCH_MISSES"};
  event_list.insert(std::end(event_list), std::begin(common_sw_counters), std::end(common_sw_counters));

  PerfProfiler myperf(&counter_name_map, &event_list, NULL);

  return myperf;
}

PerfProfiler CFG_CYCLES_TLB() {
  std::map<perf_type_config_t, std::string> counter_name_map;
  std::vector<std::string> event_list;

  event_list = {"perf::PERF_COUNT_HW_CPU_CYCLES", "perf::PERF_COUNT_HW_CACHE_DTLB:READ:ACCESS", "perf::PERF_COUNT_HW_CACHE_DTLB:READ:MISS"};
  event_list.insert(std::end(event_list), std::begin(common_sw_counters), std::end(common_sw_counters));

  PerfProfiler myperf(&counter_name_map, &event_list, NULL);

  return myperf;
}

PerfProfiler get_perf_config(std::string counter_group) {
  if (counter_group == "CFG_L1_LL") {
    return CFG_L1_LL();
  } else if (counter_group == "CFG_BRANCHES") {
    return CFG_BRANCHES();
  } else if (counter_group == "CFG_CYCLES_TLB") {
    return CFG_CYCLES_TLB();
  } else {
    return NULL;
  }
}