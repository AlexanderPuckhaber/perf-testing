// Test program for perf probe
// taken from https://bristot.me/using-perf-probe-to-measure-execution-time-of-user-space-code-on-linux/
// also see: http://blog.vmsplice.net/2011/03/how-to-use-perf-probe.html and man page for how to delete probes
// https://opensource.com/article/18/7/fun-perf-and-python has a walkthrough of perf python scripts
// (however, kernel/perf must be built from source to enable python scripts on Ubuntu...)

// to compile with debug symbols: gcc -g -o evenodd evenodd.cc
#include <stdio.h>

void even(void)
{
     printf("it is even\n");
}

void odd(void)
{
     printf("it is odd\n");
}
int main()
{
     int i;
     for (i = 0; ;i++) {
          if (i % 2)
               odd();
          else
               even();
     }
}