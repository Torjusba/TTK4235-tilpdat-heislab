/**
  @file
  @brief The elevator state machine module

*/
#ifndef _ELEVATOR_H
#define _ELEVATOR_H

#include <stdio.h>
#include <stdlib.h>
#include "driver/hardware.h"
#include "orders_test.h"
#include "timer.h"


// How long should the elevator door stay open?
#define ELEVATOR_DOOR_WAIT_TIME 3


/**
  @enum ElevatorState
  @brief Describes the state of the elevator FSM

  */
typedef enum {
  ELEVATOR_STATE_INIT,
  /** Init state, unknown position, so moves down */
  ELEVATOR_STATE_INIT_MOVING_DOWN,
  /** Idle state, no pending targets*/
  ELEVATOR_STATE_IDLE, 
  /** Elevator moving down towards a target*/
  ELEVATOR_STATE_MOVING_DOWN, 
  /** Elevator moving up towards a target*/
  ELEVATOR_STATE_MOVING_UP, 
  /** Elevator stopped at a target floor, with door closed*/
  ELEVATOR_STATE_STOPPED_AT_FLOOR, 
  /** Elevator stopped at a target floor, with door open*/
  ELEVATOR_STATE_OPEN, 
  /** Elevator stopped at a target floor, with door open and obstructed*/
  ELEVATOR_STATE_OBSTRUCTION, 
  /** Emergency stop button pressed, elevator standing still */
  ELEVATOR_STATE_ESTOP, 
  /** Emergency stop button pressed, elevator standing still at floor with open door */
  ELEVATOR_STATE_ESTOP_OPEN 
} ElevatorState;

static ElevatorState m_elevator_current_state;
static int m_elevator_current_target;
static int m_elevator_current_floor;
static int m_elevator_last_floor;


/**
  @brief Initializes the main elevator control loop

  @returns 0 on success, 1 on fail
  */
int main(int argc, char **argv);

/**
  @brief Initializes the elevator FSM from an undefined state. To be used when initially starting the elevator

  @returns 0 on success, 1 on fail
  */
int elevator_init();

/**
  @brief Reads all button inputs from hardware.h, and sends pressed buttons to orders.h

  @returns 0 on success, 1 on fail
  */
int elevator_update_orders();

/**
  @brief Checks transition conditions for the elevator FSM, and transitions if they are met

  @returns 0 on success, 1 on fail
  */
int elevator_update_state();

/** Internal */
int elevator_update_floor_status();

/** Internal */
int elevator_clear_target();

/** Internal */
int elevator_add_order_if_button_pressed(int floor, HardwareOrder hardware_order);

#endif //_ELEVATOR_H
