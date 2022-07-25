#include "tlb_test.h"

#define PAGE_SIZE 4096
#define TLB_SIZE 32

// from https://github.com/torvalds/test-tlb
static void randomize_map(void *map, unsigned long size, unsigned long stride)
{
	unsigned long off;
	unsigned int *lastpos, *rnd;
	int n;

	rnd = (unsigned int*) calloc(size / stride + 1, sizeof(unsigned int));
	if (!rnd)
		exit(1);

	/* Create sorted list of offsets */
	for (n = 0, off = 0; off < size; n++, off += stride)
		rnd[n] = off;

	/* Randomize the offsets */
	for (n = 0, off = 0; off < size; n++, off += stride) {
		unsigned int m = (unsigned long)random() % (size / stride);
		unsigned int tmp = rnd[n];
		rnd[n] = rnd[m];
		rnd[m] = tmp;
	}

	/* Create a circular list from the random offsets */
	lastpos = (unsigned int*) map;
	for (n = 0, off = 0; off < size; n++, off += stride) {
		lastpos = (unsigned int*) map + rnd[n] / sizeof(unsigned int);
		*lastpos = rnd[n+1];
	}
	*lastpos = rnd[0];

	free(rnd);
}


// from https://github.com/torvalds/test-tlb
// Hugepage size
#define HUGEPAGE (2*1024*1024)

static void *create_map(void *map, unsigned long size, unsigned long stride)
{
	unsigned int flags = MAP_PRIVATE | MAP_ANONYMOUS;
	unsigned long off, mapsize;
	unsigned int *lastpos;

	if (map) {
		flags |= MAP_FIXED;
	}

	mapsize = size;

	map = mmap(map, mapsize, PROT_READ | PROT_WRITE, flags, -1, 0);
	if (map == MAP_FAILED)
		exit(1);


  /*
    * Christian Borntraeger tested on an s390, and had
    * transparent hugepages set to "always", which meant
    * that the small-page case never triggered at all
    * unless you explicitly ask for it.
    */
  madvise(map, mapsize, MADV_NOHUGEPAGE);

	lastpos = (unsigned int*) map;
  
	for (off = 0; off < size; off += stride) {
    // dividing by 4 because of some issue, maybe with comparing bytes to unsigned int?
		lastpos = (unsigned int*) map + off / sizeof(unsigned int);
    
		*lastpos = off + stride;
    // printf("debug: lastpos: 0x%x [%x] 0x%x  0x%x 0x%x 0x%x\n", lastpos, *lastpos, map + off, off + stride, map, map + size);
	}
	*lastpos = 0;

	return map;
}

int dtlb_test(unsigned long size, unsigned long stride, unsigned int num_accesses, bool randomize, PerfProfiler *myperf) {
  // How big? let's just allocate memory for every page in DTLB, twice over
  unsigned long mapsize = size;
  void *map;

  // map = mmap(map, mapsize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  // if (map == MAP_FAILED) {
  //   fprintf(stderr, "mmap failed: %s\n", strerror(errno));
  //   return -1;
  // }

  // now, do something
  unsigned int accesses_per_page = PAGE_SIZE / stride;

  map = create_map(map, mapsize, stride);
  if (randomize)
    randomize_map(map, mapsize, stride);

  // printf("num_runs: %d stride: %d acc_per_page: %d acc_per_run: %d\n", num_runs, stride, accesses_per_page, accesses_per_run);
  // https://stackoverflow.com/questions/33167998/perf-reports-misses-larger-than-total-accesses
  // printf("est dtlb accesses: %d misses: %d\n", accesses_per_run * num_runs, (accesses_per_run * num_runs) / accesses_per_page);

  // https://stackoverflow.com/questions/15908835/how-to-store-a-c-variable-in-a-register
  // putting frequently used values in registers (as a compiler hint) reduces the memory accesses
  // also, using optimization level -O2
  register std::byte val;

  // https://stackoverflow.com/questions/52572922/what-causes-the-dtlb-load-misses-walk-performance-events-to-occur
  std::byte sum;

  unsigned int count = 0;
  unsigned int offset = 0;

  myperf->Initialize();

  myperf->Reset();
  myperf->Enable();

  
  // for (register unsigned int i = 0; i < num_runs; i++) {
  //   for (register unsigned int j = 0; j < mapsize; j+= stride) {
  //     val = *(std::byte *)(map + j);
  //     sum |= val;
  //   }
  // }

  // from https://github.com/torvalds/test-tlb
  for (uint i = 0; i < num_accesses; i++) {
		count++;
		offset = *(unsigned int *) (map + offset);
	}

  myperf->Disable();

  // Make sure the compiler doesn't compile away offset
  *(volatile unsigned int *)(map + offset);

  // printf("res=%d count=%d offset_mask=0x%x\n", sum, count, offset_mask);
  // printf("est dtlb accesses: %d misses: %d\n", count, count / (accesses_per_page));

  return (int) sum;
}