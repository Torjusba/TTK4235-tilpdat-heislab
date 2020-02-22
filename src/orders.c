#include "orders.h"


void init_orders(){
  orders_up = vector_create(); 
  orders_down = vector_create();
  target_buffer = vector_create();
  target = -1; 
}



int orders_get_new_target(HardwareMovement hardware_movement, int curr_floor){
  if (hardware_movement == HARDWARE_MOVEMENT_UP){
    int orders_up_size = vector_size(orders_up);
    //Going through orders_up to see if we can do some stops on the way
    //And adding them to target

    for (int i = 0; i < orders_up_size; i++){
      if (curr_floor < orders_up[i] && target > orders_up[i]){
	target = orders_up[i]; 
	vector_remove(orders_up, i);
	update_target_buffer(target);
	i--;
	orders_up_size--;
      }
    }
  }
  else if (hardware_movement == HARDWARE_MOVEMENT_DOWN){
    int orders_down_size = vector_size(orders_down);
    //Going through orders_down to see if we can do some stops on the way
    //And adding them to targets
    for (int i = 0; i < orders_down_size; i++){
      if (curr_floor > orders_down[i] && target < orders_down[i]){
	target = orders_down[i]; 
	vector_remove(orders_down, i);
	update_target_buffer(target);
	i--;
	orders_down_size--;
      }
    }
  }

  //Checking if target is empty, and new target have to be included. 
  if (target == -1){
    int buffer_size = vector_size(target_buffer);
    if (buffer_size > 0){
      bubbleSort(target_buffer, buffer_size);
      target = target_buffer[0];
    }
    else if (buffer_size == 0 && target == -1){
      //Find closest order, and set to target
      int orders_up_size = vector_size(orders_up);
      int orders_down_size = vector_size(orders_down);

      if (orders_up_size == 0 && orders_down_size == 0)
      {
	return -1;
      } else if (orders_down_size == 0)
      {
	// Only orders up
	//Find closest floor to the elevator
	int closest_floor_up = orders_up[0];	
	int order_up_index = 0;
	for (int i = 1; i < orders_up_size; i++){
	  if (abs(closest_floor_up - curr_floor) > abs(orders_up[i] - curr_floor)){

	    closest_floor_up = orders_up[i];
	    order_up_index = i;
	  }
	}
	vector_remove(orders_up, order_up_index);
	target = closest_floor_up;
	return target;


      } else if (orders_up_size == 0)
      {
	// Only orders down
	//Find closest floor to the elevator
	int closest_floor_down = orders_down[0];	
	int order_down_index = 0;
	for (int i = 1; i < orders_down_size; i++){
	  if (abs(closest_floor_down - curr_floor) > abs(orders_down[i] - curr_floor)){

	    closest_floor_down = orders_down[i];
	    order_down_index = i;
	  }
	}
	vector_remove(orders_down, order_down_index);
	target = closest_floor_down;
	return target;


      } else 
      {
	// Both orders up and down
	int closest_floor_up = orders_up[0];
	int closest_floor_down = orders_down[0];
	int order_up_index;
	int order_down_index;

	//Find closest floor to the elevator
	for (int i = 1; i < orders_up_size; i++){
	  if (abs(closest_floor_up - curr_floor) > abs(orders_up[i] - curr_floor)){

	    closest_floor_up = orders_up[i];
	    order_up_index = i;
	  }
	}
	for (int i = 1; i < orders_down_size; i++){
	  if (abs(closest_floor_down - curr_floor) > abs(orders_down[i] - curr_floor)){

	    closest_floor_down = orders_down[i];
	    order_down_index = i; 
	  }
	}
	if (abs(closest_floor_down - curr_floor) < abs(closest_floor_up-curr_floor)){
	  target = closest_floor_down;
	  vector_remove(orders_down, order_down_index);
	}
	else if (abs(closest_floor_down - curr_floor) > abs(closest_floor_up-curr_floor)){
	  target = closest_floor_up;
	  vector_remove(orders_up, order_up_index);
	}
	else if (abs(closest_floor_down - curr_floor) == abs(closest_floor_up-curr_floor)){
	  target = closest_floor_up;
	  vector_remove(orders_up, order_up_index); 
	}
      }
    }
  }
  return target; 

}

void orders_add_order(HardwareOrder hardware_order, int floor){
  switch (hardware_order)
  {
    case HARDWARE_ORDER_UP:
      update_orders_up(floor);
      break;
    case HARDWARE_ORDER_DOWN:
      update_orders_down(floor);
      break;
    case HARDWARE_ORDER_INSIDE:
      update_target_buffer(floor);
      break;
    default:
      break;
  }
  return;
}

void update_target_buffer(int floor){
  int target_buffer_size = vector_size(target_buffer);
  for (int i = 0; i < target_buffer_size; i++)
  {
    //Don't add duplicates
    if (target_buffer[i] == floor)
      return;
  }
  vector_add(&target_buffer, floor);
}


//Updating the list orders_up when the up button is pressed on some floor
void update_orders_up(int floor){
  int orders_up_size = vector_size(orders_up);
  for (int i = 0; i < orders_up_size; i++)
  {
    //Don't add duplicates
    if (orders_up[i] == floor)
      return;
  }
  vector_add(&orders_up, floor);
}

//Updating the list orders_down when the down button is pressed on some floor
void update_orders_down(int floor){
  int orders_down_size = vector_size(orders_down);
  for (int i = 0; i < orders_down_size; i++)
  {
    //Don't add duplicates
    if (orders_down[i] == floor)
      return;
  }
  vector_add(&orders_down, floor);
}


void orders_clear_target(int floor){
  int orders_up_size = vector_size(orders_up);
  for(int i = 0; i < orders_up_size; i++)
  {
    if (orders_up[i] == floor)
    {
      vector_remove(orders_up, i--);
      orders_up_size--;
    }
  }
  int orders_down_size = vector_size(orders_down);
  for(int i = 0; i < orders_down_size; i++)
  {
    if (orders_down[i] == floor)
    {
      vector_remove(orders_down, i--);
      orders_down_size--;
    }
  }
  int target_buffer_size = vector_size(target_buffer);
  for(int i = 0; i < target_buffer_size; i++)
  {
    if (target_buffer[i] == floor)
    {
      vector_remove(target_buffer, i--);
      target_buffer_size--;
    }
  }
  if (target == floor)
  {
    target = -1;
  }
  return;
}


void orders_clear_all(){
  vector_free(orders_up);
  vector_free(orders_down);
  vector_free(target_buffer);
  init_orders(); 
}

void swap(int *xp, int *yp)  
{  
  int temp = *xp;  
  *xp = *yp;  
  *yp = temp;  
}  

// A function to implement bubble sort  
void bubbleSort(int arr[], int n)  
{  
  int i, j;  
  for (i = 0; i < n-1; i++)      

    // Last i elements are already in place  
    for (j = 0; j < n-i-1; j++)  
      if (arr[j] > arr[j+1])  
	swap(&arr[j], &arr[j+1]);  
}  
