# Some short c++ programs I've been playing around with

## `perf_profiler.cc` and `perf_profiler.h`
Uses `perf_event_open` API to record hardware and software event counters.

`PerfProfiler` object constructors take these parameters:
- `std::map<perf_type_config_t, std::string> *perf_counter_name_map;`
  - See `/usr/include/linux/perf_event.h` for the event types and configs
  - Some flags require offsets, see this [stackoverflow example](https://stackoverflow.com/questions/61190033/how-to-measure-the-dtlb-hits-and-dtlb-misses-with-perf-event-open)
- `std::vector<std::string> *search_perf_event_names;`
  - Event names that libpfm4 can search for
  - You can use libpfm4 [`check_events`](https://github.com/wcohen/libpfm4/tree/master/examples) program to [find the full event name](https://web.eece.maine.edu/~vweaver/projects/perf_events/faq.html#q2e)
- `perf_event_attr *perf_event_attribute_default`
  - If NULL, this is hardcoded in `perf_profiler.cc`

The map or vector can be empty, but at least *one* event must be supplied.

The constructor uses these to generate perf event attributes.

To use:

`PerfProfiler::Initialize()`

Syscalls have the kernel load the perf event attributes.

If any event attribute is invalid, the program will terminate.

`PerfProfiler::Reset()`

Sets counters to 0

`PerfProfiler::Enable()`

Enables counters as a group. You should call this immediately before the code you want to profile.

`PerfProfiler::Disable()`

Disables (stops) counters as a group. You should call this immediately after the code you want to profile.

`PerfProfiler::Read()`

Reads counters and returns them as a map of `std::map<std::string, uint64_t>`, 
where the string is either the name you supplied in `perf_counter_name_map`, 
or the full name libpfm4 found when searching the full/partial event name you supplied in `search_perf_event_names`.

## Makefile
Install [`libpfm4`](https://github.com/wcohen/libpfm4)

## Programs:
In `src/` folder, run `make all`

Output binaries are in `bin/`
### `perf_matmul_test`
- Program multiplies matrices: A (I x K), B (K x J) to output C (I x J)
- `make perf-matmul-test` --> `/bin/perf_matmul_test`
- usage: -i I -j J -k K -m METHOD (naive, tiled) -b BLOCK_SIZE
- Output: `PerfProfiler::Read()` for a hardcoded list of counters found in `perf_matmul_test.cc`
- Counters are only enabled during the tiled matrix multiplication (not for the array initializations)
- Tiled works but naiive is broken. Just choose a really big tiling factor to get close to naiive implementation.
### `perf_tlb_test` (currently needs to be fixed due to change with my perf wrapper)
From [https://github.com/torvalds/test-tlb](https://github.com/torvalds/test-tlb)
- Program allocates memory "map" for a given `mapsize`, then makes `num_accesses` reads at every memory location in the map offset by `stride`, either sequentially or in randomized order.
- `make perf-tlb-test` --> `/bin/perf_tlb_test`
- usage: -m mapsize -s stride -n num_accesses -r randomize -c cache_level
  - randomize: {0,1} cache_level: {L1,L2,LL}
- Output: `PerfProfiler::Read()` for a hardcoded list of counters found in `perf_matmul_test.cc`
- Counters are only enabled during the memory accesses, not the allocation. The random order is also computed beforehand, and the order is saved in the map and read during the memory access of the map.
- The `cache_level` is specified in the driver code because only 3-4 HW Cache counters can be enabled simultaneously on my Intel PMU. So, the relationship between DTLB misses and L1-Data, L2, and LL Cache misses can be measured by taking many samples of each and comparing the averages.

### Scripts
Python scripts to run these binaries and save the results

#### `experiments_matmul.py` and `experiments_tlb.py`
- Run the binaries with a range of parameters and number of samples
- Saves results to `.csv` file in `data/` folder

#### `analysis_matmul.py` and `analysis_tlb.py`
- Read results of experiments
- Produces visualizations using pyplot and seaborn for heatmaps
  - Good way to see relationships between parameters and result counters