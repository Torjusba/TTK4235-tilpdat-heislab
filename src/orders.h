#ifndef orders_h 
#define orders_h 

#include "vec.h"
#include "driver/hardware.h"

//Some initializing if needed
void init_orders(); 

//Updating the list target_buffer
void update_target_buffer(int floor); 

//Updating the list orders_up when the up button is pressed on some floor
void update_orders_up(int floor);

//Updating the list orders_down when the down button is pressed on some floor
void update_orders_down(int floor); 


void orders_add_order(HardwareOrder hardware_order, int floor);

void orders_clear_target(int floor);

void orders_clear_all(); 


int orders_get_new_target(HardwareMovement hardware_movement, int curr_floor);


void bubbleSort(int arr[], int n); 
void swap(int *xp, int *yp); 



int* orders_up  ;
int* orders_down; 
int* targets; 
int* target_buffer; 
int target; 



#endif
