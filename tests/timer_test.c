#include "timer.h"
#include <stdio.h>

int main(int argc, char **argv)
{
  int timer_limit;
  if(argc <= 1)
  {
    printf("No time limit specified, defaulting to 3 seconds\n");
    timer_limit = 3;
  } else
  {
    timer_limit = (int)(*argv[1] - '0');
  }

  timer_reset();
  int seconds = 0;
  do
  {
    seconds = timer_get_seconds();
    printf("\rElapsed seconds: %d / %d", seconds, timer_limit);
  } while (seconds < timer_limit);
  return 0;
}
