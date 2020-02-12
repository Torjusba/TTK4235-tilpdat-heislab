#include "timer.h"
#include <stdio.h>

int main(int argc, char **argv)
{
  double timer_limit;
  if(argc <= 1)
  {
    printf("No time limit specified, defaulting to 3 seconds\n");
    timer_limit = 3.00;
  } else
  {
    timer_limit = (double)(*argv[1] - '0');
  }

  timer_reset();
  double seconds = 0;
  do
  {
    seconds = timer_get_seconds();
    printf("\rElapsed seconds: %f / %f", seconds, timer_limit);
  } while (seconds < timer_limit);
  return 0;
}
