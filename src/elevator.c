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



    while(1){
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
      if(floor != m_elevator_current_floor)
      {
	hardware_command_floor_indicator_on(floor);
	m_elevator_current_floor = floor;
	m_elevator_last_floor = floor;
      }
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
      // Uses last_floor instead of current_floor because the elevator can be stopped
      // by the stop button outside of a floor
      if (m_elevator_current_target != -1)
      {
	if (m_elevator_current_target != m_elevator_last_floor)
	{
	  if (m_elevator_current_target < m_elevator_last_floor)
	  {
	    hardware_command_movement(HARDWARE_MOVEMENT_DOWN);
	    m_elevator_current_state = ELEVATOR_STATE_MOVING_DOWN;
	  } else
	  {
	    hardware_command_movement(HARDWARE_MOVEMENT_UP);
	    m_elevator_current_state = ELEVATOR_STATE_MOVING_UP;
	  }
	}else
	{
	  // Elevator is already at target
	  // TODO: Remove target
	  // orders_complete_floor(int m_elevator_last_floor);
	}
      }
      break;

    case ELEVATOR_STATE_MOVING_DOWN:
      if (m_elevator_current_target == m_elevator_last_floor)
      {
	hardware_command_movement(HARDWARE_MOVEMENT_STOP);
	m_elevator_current_state = ELEVATOR_STATE_STOPPED_AT_FLOOR;
      }
      break;

    case ELEVATOR_STATE_MOVING_UP:
      if (m_elevator_current_target == m_elevator_last_floor)
      {
	hardware_command_movement(HARDWARE_MOVEMENT_STOP);
	m_elevator_current_state = ELEVATOR_STATE_STOPPED_AT_FLOOR;
      }
      break;

    case ELEVATOR_STATE_STOPPED_AT_FLOOR:
      if (m_elevator_current_target == m_elevator_last_floor)
      {
	//TODO: clear target properly
	m_elevator_current_target = -1;
	hardware_command_door_open(1);
	//TODO: timer_reset();
	m_elevator_current_state = ELEVATOR_STATE_OPEN;
      } else 
      {
	m_elevator_current_state = ELEVATOR_STATE_IDLE;
      }
      break;

    case ELEVATOR_STATE_OPEN:
      if (hardware_read_obstruction_signal())
      {
	m_elevator_current_state = ELEVATOR_STATE_OBSTRUCTION;
      } else
      {
	//TODO: Check if 3 seconds has passed
	hardware_command_door_open(0);
	m_elevator_current_state = ELEVATOR_STATE_STOPPED_AT_FLOOR;
      }
      break;

    case ELEVATOR_STATE_OBSTRUCTION:
      if (!hardware_read_obstruction_signal())
      {
	m_elevator_current_state = ELEVATOR_STATE_OPEN;
      }
      break;

    case ELEVATOR_STATE_ESTOP:
      if (m_elevator_current_floor != -1)
      {
	hardware_command_door_open(1);
	m_elevator_current_state = ELEVATOR_STATE_ESTOP_OPEN;
      } else if (!hardware_read_stop_signal())
      {
	m_elevator_current_state = ELEVATOR_STATE_IDLE;
      }
      break;

    case ELEVATOR_STATE_ESTOP_OPEN:
      if (!hardware_read_stop_signal())
      {
	m_elevator_current_state = ELEVATOR_STATE_OPEN;
      }
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
  return 0;
}

/** For internal use only. Called when the elevator reaches its target */
int elevator_clear_target()
{
  hardware_command_order_light(m_elevator_last_floor, HARDWARE_ORDER_DOWN, 0);
  hardware_command_order_light(m_elevator_last_floor, HARDWARE_ORDER_INSIDE, 0);
  hardware_command_order_light(m_elevator_last_floor, HARDWARE_ORDER_UP, 0);

  //orders_clear_floor(m_elevator_last_floor);

  m_elevator_current_target = -1;
  return 0;
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

