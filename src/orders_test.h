// Temporary header for testing the FSM

#ifndef ORDERS_TEST_H
#define ORDERS_TEST_H

#include <stdio.h>
#include <stdlib.h>

int orders_add_order(HardwareOrder hardware_order, int floor)
{
  printf("Direction: %d, Floor, %d\n", (int)hardware_order, floor);
  return 0;
}



#endif //ORDERS_TEST_H
