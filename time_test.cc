// Short program to print time.h CLOCK_MONOTONIC_RAW
// Was put in onnxruntime

#include <time.h>
#include <string>

int main () {
  struct timespec time_start;

  clock_gettime(CLOCK_MONOTONIC_RAW, &time_start);

  char kernel_begin_time_monotonic_raw_buffer [50];
  sprintf(kernel_begin_time_monotonic_raw_buffer, "%d.%06ld\n",
    time_start.tv_sec, time_start.tv_nsec / 1000);

  printf("%d.%06ld\n", time_start.tv_sec, time_start.tv_nsec / 1000);
  printf("%s\n", std::to_string(kernel_begin_time_monotonic_raw_buffer));

  std::string blah;
  // blah = std::to_string(time_start);
}