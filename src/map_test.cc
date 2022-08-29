#include "get_perf_configs.h"
#include <sys/mman.h>

#define PAGE_SIZE 4096
#define TLB_SIZE 32

// from https://stackoverflow.com/questions/53886131/how-does-xorshift32-works
typedef struct {
    uint64_t  state;
} prng_state;

static inline uint64_t prng_u64(prng_state *const p)
{
    uint64_t  state = p->state;
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    p->state = state;
    return state * UINT64_C(2685821657736338717);
}


typedef struct {
  std::vector<unsigned int> block_cursors;
  unsigned int num_accesses;
} block_access_info;

int main(int argc, char *argv[]) {
  int c;
  std::string perf_command;

  while ((c = getopt(argc, argv, "i:j:k:m:b:p:")) != -1) {
    switch (c) {
      case 'p':
        perf_command = std::string(optarg);
        break;
      case 'h':
        // fall through
      default:
        printf("help: -p {CFG_L1_LL, CFG_BRANCHES, CFG_CYCLES_TLB}");
        return -1;
    }
  }

  PerfProfiler myperf = get_perf_config(perf_command);

  // setup program

  unsigned int num_pages = 64;
  unsigned int mapsize = num_pages * PAGE_SIZE;
  unsigned int num_accesses = 1024*16;

  // std::vector<block_access_info> block_info(num_pages);

  void *map;

  map = mmap(map, mapsize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);


  myperf.Initialize();
  myperf.Reset();
  myperf.Enable();

  // run program
  unsigned int val = 0;
  unsigned int i;
  unsigned int idx = 0;

  unsigned int page_number = 0;
  unsigned int page_offset = 0;

  unsigned int accesses_in_page = 0;
  unsigned int accesses_per_page_offset = 4096*2;

  prng_state p;
  p.state = 0;

  for (i = 0; i < num_accesses; i++) {
    idx = (page_number) * PAGE_SIZE + page_offset;
    val = val | *(unsigned int*) (map + (idx % mapsize));
    page_offset += 1;

    if (i % accesses_per_page_offset == 0) {
      p.state = i;
      page_number = prng_u64(&p) % num_pages;
    }
  }

  *(unsigned int*) (map + (idx % mapsize)) = val;
  

  myperf.Disable();

  std::map<std::string, uint64_t> result_map = myperf.Read();
  result_map = myperf.Read();

  for (const auto& kv : result_map) {
    printf("%s: %s\n", kv.first.c_str(), std::to_string(kv.second).c_str());
  }

  return 0;
}