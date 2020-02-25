/**
   @file
   @brief A module for keeping track of orders
   */
#ifndef orders_h 
#define orders_h 

#include <math.h>
#include "vec.h"
#include "driver/hardware.h"

/**
  * @brief Initializes the orders module. Only needs to be called once at the beginning of the program
  */
void init_orders(); 

/**
  * @brief Adds the floor to the orders_target buffer, but discards duplicates
  * 
  * This function is primarily for internal use, and programs using the module should call orders_add_order instead
  * @param floor The floor number to add as a orders_target. 0-indexed
  */
void orders_add_to_target_buffer(int floor); 

/**
  * @brief Adds the floor to the orders_up buffer, but discards duplicates
  * @param floor The floor number to add. 0-indexed
  */
void orders_add_to_orders_up(int floor);

/**
  * @brief Adds the floor to the orders_down buffer, but discards duplicates
  * @param floor The floor number to add. 0-indexed
  */
void orders_add_to_orders_down(int floor); 



/**
  * @brief Generic wrapper function for adding orders. This is the recommended way to add orders
  * @param hardware_order The order type (Which panel was the elevator ordered from?)
  * @param floor The floor number to add. 0-indexed
  */
void orders_add_order(HardwareOrder hardware_order, int floor);


/**
  * @brief Clears the current orders_target. To be used when the elevator successfully handles an order
  * @param floor The floor number to clear. 0-indexed
  */
void orders_clear_target(int floor);

/**
  * @brief Clears all entries from all buffers. To be called when the elevator makes an emergency stop
  */
void orders_clear_all(); 


/**
  * @brief Requests a new orders_target from the buffer
  * @param hardware_movement The current movement direction of the elevator
  * @param current_floor The current posision of the elevator. 0-indexed. This is a double because the elevator can be between floors after an emergency stop
  * @returns The floor number of the floor the elevator should move to
  */
int orders_get_new_target(HardwareMovement hardware_movement, double current_floor);


void bubbleSort(int arr[], int n); 
void swap(int *xp, int *yp); 



int* orders_up  ;
int* orders_down; 
int* orders_target_buffer; 
int orders_target; 



#endif
