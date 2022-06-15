## Some short c++ programs I've been playing around with

### `perf_test.cc`
Taken from a [StackOverflow post](https://stackoverflow.com/questions/42088515/perf-event-open-how-to-monitoring-multiple-events).
It shows how to use perf's [`perf_event_open` API](https://linux.die.net/man/2/perf_event_open) to:
- Create an event group
- Reset the event counters
- Run perf on a given process(s) and cpu(s)
- Read event counters
- Stop perf

### `perf_profiler.cc` and `perf_profiler.h`
I want to use the `perf_event_open` API to record event counters for each "layer" in an onnxruntime run.

These files have some skeleton code I want to fill out and incorporate into onnxruntime. I think the perf event counting setup can be done once per session... like how the [builtin onnxruntime profiler](https://github.com/microsoft/onnxruntime/blob/master/onnxruntime/core/common/profiler.cc) works.

In [`sequential_executor.cc`](https://github.com/microsoft/onnxruntime/blob/master/onnxruntime/core/framework/sequential_executor.cc), it would reset/start the event counters just before a layer operation function is called, and read them when that function returns.

Then, the event counter values can be passed along to the builtin onnxruntime profiler, to be saved to a json. An example of that can be found [here](https://github.com/microsoft/onnxruntime/compare/master...AlexanderPuckhaber:profiler_monotonic_clock).

### `evenodd.cc`
Taken from a [StackOverflow post](https://bristot.me/using-perf-probe-to-measure-execution-time-of-user-space-code-on-linux/)

You can read it to learn how `perf probe` works. Perf probe can set tracepoints for individual lines of code in binaries, which might be useful. Ideally, we would just have to compile onnxruntime with debug symbols *once* and then we could use lines in onnxruntime as events! Then, we might be able to use standalone code like `perf_test.cc` to setup perf with that event as the "group leader", and read out the counters when the function is called or when the function returns!

### `time_test.cc`
Was just testing how to use `<time.h>` CLOCK_MONOTONIC_RAW