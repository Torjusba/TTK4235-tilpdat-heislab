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
	    fprintf(stdout, "STOP command received, aborting...\n");
            break;
        }

	if(elevator_update_orders())
	{
	  fprintf(stderr, "Unable to upate orders");
	}

	if (elevator_update_floor_status())
	{
	  fprintf(stderr, "Unable to update floor status");
	}

	if(elevator_update_state())
	{
	  fprintf(stderr, "Unable to update state");
	}
    }
    return 0;
}


int elevator_init()
{
  m_elevator_current_state = ELEVATOR_STATE_INIT;
  m_elevator_current_floor = -1;
  m_elevator_current_target = -1;
  return 0;
}

int elevator_update_floor_status()
{
  for (int floor = 0; floor < HARDWARE_NUMBER_OF_FLOORS; floor++)
  {
    if (hardware_read_floor_sensor(floor))
    {
      m_elevator_current_floor = floor;
      m_elevator_last_floor = floor;
      return 0;
    }
  }
  m_elevator_current_floor = -1;
  return 0;
}

int elevator_update_state()
{
  switch (m_elevator_current_state)
  {
    case ELEVATOR_STATE_INIT:
      hardware_command_movement(HARDWARE_ORDER_DOWN);
      m_elevator_current_state = ELEVATOR_STATE_INIT_MOVING_DOWN;
      break;
    case ELEVATOR_STATE_INIT_MOVING_DOWN:
      if (m_elevator_current_floor != -1)
      {
	hardware_command_movement(HARDWARE_MOVEMENT_STOP);
	m_elevator_current_state = ELEVATOR_STATE_IDLE;
      }
      break;
    case ELEVATOR_STATE_IDLE:
      break;
    case ELEVATOR_STATE_MOVING_DOWN:
      break;
    case ELEVATOR_STATE_MOVING_UP:
      break;
    case ELEVATOR_STATE_STOPPED_AT_FLOOR:
      break;
    case ELEVATOR_STATE_OPEN:
      break;
    case ELEVATOR_STATE_OBSTRUCTION:
      break;
    case ELEVATOR_STATE_ESTOP:
      break;
    case ELEVATOR_STATE_ESTOP_OPEN:
      break;
    default:
      fprintf(stderr, "Invalid state %d", m_elevator_current_state);
      return -1;
  }
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

