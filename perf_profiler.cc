// Skeleton code for a c++ class to be incorporated into onnxruntime
// Want to make it easy to use perf API to initialize an event group, start, read, and reset counters.

// https://linux.die.net/man/2/perf_event_open
// https://stackoverflow.com/questions/42088515/perf-event-open-how-to-monitoring-multiple-events
#include "perf_profiler.h"

namespace onnxruntime {
namespace profiling {


void profiling::PerfProfiler::Initialize() {
  
}

void profiling::PerfProfiler::StartResetCounters() {

}

void profiling::PerfProfiler::ReadCounters() {

}

}  // namespace profiling
}  // namespace onnxruntime