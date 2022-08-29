#include "get_perf_configs.h"

std::map<std::string, std::string> common_sw_counters = {
  {"perf::PERF_COUNT_SW_PAGE_FAULTS", "page-faults"}, 
  {"perf::PERF_COUNT_SW_CONTEXT_SWITCHES", "context-switches"},
  {"perf::PERF_COUNT_SW_CPU_MIGRATIONS", "cpu-migrations"},
  {"perf::PERF_COUNT_SW_TASK_CLOCK", "task-clock"}
};

PerfProfiler CFG_L1_LL() {
  std::map<perf_type_config_t, std::string> counter_name_map;
  std::map<std::string, std::string> event_list;

  event_list = {
    {"L1-DCACHE-LOADS", "L1-dcache-loads"}, 
    {"L1-DCACHE-LOAD-MISSES", "L1-dcache-load-misses"},
    {"PERF_COUNT_HW_CACHE_LL:READ:ACCESS", "cache-references"},
    {"PERF_COUNT_HW_CACHE_LL:READ:MISS", "cache-misses"}
  };
  event_list.insert(std::begin(common_sw_counters), std::end(common_sw_counters));

  PerfProfiler myperf(&counter_name_map, &event_list, NULL);

  return myperf;
}

PerfProfiler CFG_BRANCHES() {
  std::map<perf_type_config_t, std::string> counter_name_map;
  std::map<std::string, std::string> event_list;

  event_list = {
    {"perf::PERF_COUNT_HW_INSTRUCTIONS", "instructions"}, 
    {"perf::PERF_COUNT_HW_BRANCH_INSTRUCTIONS", "branches"},
    {"perf::PERF_COUNT_HW_BRANCH_MISSES", "branch-misses"}
  };
  event_list.insert(std::begin(common_sw_counters), std::end(common_sw_counters));

  PerfProfiler myperf(&counter_name_map, &event_list, NULL);

  return myperf;
}

PerfProfiler CFG_CYCLES_TLB() {
  std::map<perf_type_config_t, std::string> counter_name_map;
  std::map<std::string, std::string> event_list;

  event_list = {
    {"perf::PERF_COUNT_HW_CPU_CYCLES", "cycles"},
    {"perf::PERF_COUNT_HW_CACHE_DTLB:READ:ACCESS", "L1-dcache-loads"},
    {"perf::PERF_COUNT_HW_CACHE_DTLB:READ:MISS", "dTLB-load-misses"}
  };
  event_list.insert(std::begin(common_sw_counters), std::end(common_sw_counters));

  PerfProfiler myperf(&counter_name_map, &event_list, NULL);

  return myperf;
}

PerfProfiler CFG_ll_TLB() {
  std::map<perf_type_config_t, std::string> counter_name_map;
  std::map<std::string, std::string> event_list;

  event_list = {
    {"PERF_COUNT_HW_CACHE_LL:READ:ACCESS", "cache-references"},
    {"PERF_COUNT_HW_CACHE_LL:READ:MISS", "cache-misses"},
    {"perf::PERF_COUNT_HW_CACHE_DTLB:READ:ACCESS", "L1-dcache-loads"},
    {"perf::PERF_COUNT_HW_CACHE_DTLB:READ:MISS", "dTLB-load-misses"}
  };
  event_list.insert(std::begin(common_sw_counters), std::end(common_sw_counters));

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
  } else if (counter_group == "CFG_LL_TLB") {
    return CFG_ll_TLB();
  } else {
    return NULL;
  }
}