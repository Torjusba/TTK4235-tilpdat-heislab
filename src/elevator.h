/**
  @file
  @brief The elevator state machine module

*/
#ifndef _ELEVATOR_H
#define _ELEVATOR_H

#include "driver/hardware.h"
#include "orders_test.h"

/**
  @enum ElevatorState
  @brief Describes the state of the elevator FSM

  */
typedef enum {
  ELEVATOR_INIT,
  /** Init state, unknown position, so moves down */
  ELEVATOR_INIT_MOVING_DOWN,
  /** Idle state, no pending targets*/
  ELEVATOR_IDLE, 
  /** Elevator moving down towards a target*/
  ELEVATOR_MOVING_DOWN, 
  /** Elevator moving up towards a target*/
  ELEVATOR_MOVING_UP, 
  /** Elevator stopped at a target floor, with door closed*/
  ELEVATOR_STOPPED_AT_FLOOR, 
  /** Elevator stopped at a target floor, with door open*/
  ELEVATOR_OPEN, 
  /** Elevator stopped at a target floor, with door open and obstructed*/
  ELEVATOR_OBSTRUCTION, 
  /** Emergency stop button pressed, elevator standing still */
  ELEVATOR_ESTOP, 
  /** Emergency stop button pressed, elevator standing still at floor with open door */
  ELEVATOR_ESTOP_OPEN 
} ElevatorState;

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

#endif //_ELEVATOR_H
