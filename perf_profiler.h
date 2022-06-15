// Supports perf_profiler.cc

#include <inttypes.h>
#include <linux/perf_event.h>
#include <list>
#include <initializer_list>


namespace onnxruntime {

namespace profiling {

// define format perf records to
struct perf_read_format {
  uint64_t nr;
  struct {
    uint64_t value;
    uint64_t id;
  } values[];
};

struct perf_value_format {
  
};

class PerfProfiler {
  public:
    PerfProfiler() noexcept {};

    ~PerfProfiler();

    std::list<perf_event_attr> pea_list;

    void Initialize();

    void StartResetCounters();

    void ReadCounters();

};

}  // namespace profiling
}  // namespace onnxruntime

