#include <stdio.h>
#include <stdlib.h>
#include "elevator.h"

int main(int argc, char **argv){
    int error = hardware_init();
    if(error != 0){
       fprintf(stderr, "unable to initialize hardware\n");
      exit(1);
    }
    error = elevator_init();
    if(error != 0){
       fprintf(stderr, "unable to initialize elevator FSM\n");
      exit(1);
    }

    printf("=== Elevator control ===\n");
    printf("Press the stop button on the elevator panel to exit\n");



    while(1){
        if(hardware_read_stop_signal()){
            hardware_command_movement(HARDWARE_MOVEMENT_STOP);
            break;
        }

	if(elevator_update_orders())
	{
	  fprintf(stderr, "Unable to upate orders");
	}

	/**
        if(hardware_read_floor_sensor(0)){
            hardware_command_movement(HARDWARE_MOVEMENT_UP);
        }
        if(hardware_read_floor_sensor(HARDWARE_NUMBER_OF_FLOORS - 1)){
            hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
        }
	*/
    }
}


int elevator_init()
{
  return 0;
}

int elevator_add_order_if_button_pressed(int floor, HardwareOrder hardware_order)
{
  if (hardware_read_order(floor, hardware_order))
  {
    orders_add_order(hardware_order, floor);
  }
}

int elevator_update_orders()
{
  for (int floor = 0; floor <= HARDWARE_NUMBER_OF_FLOORS-1; floor++)
  {
    elevator_add_order_if_button_pressed(floor, HARDWARE_ORDER_UP);
    elevator_add_order_if_button_pressed(floor, HARDWARE_ORDER_INSIDE);
    elevator_add_order_if_button_pressed(floor, HARDWARE_ORDER_DOWN);
  }
  return 0;
}

