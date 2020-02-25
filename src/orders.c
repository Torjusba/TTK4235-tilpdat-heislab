#include "orders.h"


void init_orders(){
  orders_up = vector_create(); 
  orders_down = vector_create();
  orders_target_buffer = vector_create();
  orders_target = -1; 
}



int orders_get_new_target(HardwareMovement hardware_movement, double current_floor){
  if (hardware_movement == HARDWARE_MOVEMENT_UP){
    if(orders_target == -1)
    {
      //ESTOP
      return orders_get_new_target(HARDWARE_MOVEMENT_STOP, current_floor+0.5);
    }
    int orders_up_size = vector_size(orders_up);
    //Going through orders_up to see if we can do some stops on the way
    //And adding them to orders_target
    for (int i = 0; i < orders_up_size; i++){
      if (current_floor < orders_up[i] && orders_target > orders_up[i]){
	orders_target = orders_up[i]; 
	vector_remove(orders_up, i);
	orders_add_to_target_buffer(orders_target);
	i--;
	orders_up_size--;
      }
    }
    return orders_target;
  }
  else if (hardware_movement == HARDWARE_MOVEMENT_DOWN){
    if(orders_target == -1)
    {
      //ESTOP
      return orders_get_new_target(HARDWARE_MOVEMENT_STOP, current_floor-0.5);
    }
    int orders_down_size = vector_size(orders_down);
    //Going through orders_down to see if we can do some stops on the way
    //And adding them to orders_targets
    for (int i = 0; i < orders_down_size; i++){
      if (current_floor > orders_down[i] && orders_target < orders_down[i]){
	orders_target = orders_down[i]; 
	vector_remove(orders_down, i);
	orders_add_to_target_buffer(orders_target);
	i--;
	orders_down_size--;
      }
    }
    return orders_target;
  }

  //Checking if orders_target is empty, and new orders_target have to be included. 
  
    int buffer_size = vector_size(orders_target_buffer);
    if (buffer_size > 0){
      bubbleSort(orders_target_buffer, buffer_size);
      if (hardware_movement == HARDWARE_MOVEMENT_DOWN){
        orders_target = orders_target_buffer[buffer_size];
      }else if (hardware_movement == HARDWARE_MOVEMENT_UP){
        orders_target = orders_target_buffer[0];
      }else 
      orders_target = orders_target_buffer[0]; 
    }

    else if (buffer_size == 0){
      //Find closest order, and set to orders_target
      int orders_up_size = vector_size(orders_up);
      int orders_down_size = vector_size(orders_down);

      if (orders_up_size == 0 && orders_down_size == 0)
      {
	    return -1;
      } else if (orders_down_size == 0)
      {
	// Only orders going up
	//Find closest floor to the elevator
	int closest_floor_up = orders_up[0];	
	int order_up_index = 0;
	for (int i = 1; i < orders_up_size; i++){
	  if (abs(closest_floor_up - current_floor) > abs(orders_up[i] - current_floor)){

	    closest_floor_up = orders_up[i];
	    order_up_index = i;
	  }
	}
	vector_remove(orders_up, order_up_index);
	orders_target = closest_floor_up;
	orders_add_to_target_buffer(orders_target);
	return orders_target;
      } else if (orders_up_size == 0)
      {
	// Only orders going down
	//Find closest floor to the elevator
	int closest_floor_down = orders_down[0];	
	int order_down_index = 0;
	for (int i = 1; i < orders_down_size; i++){
	  if (abs(closest_floor_down - current_floor) > abs(orders_down[i] - current_floor)){

	    closest_floor_down = orders_down[i];
	    order_down_index = i;
	  }
	}
	vector_remove(orders_down, order_down_index);
	orders_target = closest_floor_down;
	orders_add_to_target_buffer(orders_target);
	return orders_target;
      } else 
      {
	// There are both orders going up and down
	int closest_floor_up = orders_up[0];
	int closest_floor_down = orders_down[0];
	int order_up_index = 0;
	int order_down_index = 0;

	//Find closest order to the elevator
	for (int i = 1; i < orders_up_size; i++){
	  if (abs(closest_floor_up - current_floor) > abs(orders_up[i] - current_floor)){

	    closest_floor_up = orders_up[i];
	    order_up_index = i;
	  }
	}
	for (int i = 1; i < orders_down_size; i++){
	  if (abs(closest_floor_down - current_floor) > abs(orders_down[i] - current_floor)){

	    closest_floor_down = orders_down[i];
	    order_down_index = i; 
	  }
	}
	
	// Compare closest orders
	if (abs(closest_floor_down - current_floor) < abs(closest_floor_up-current_floor)){
	  orders_target = closest_floor_down;
	  orders_add_to_target_buffer(orders_target);
	  vector_remove(orders_down, order_down_index);
	  return orders_target;
	}
	else if (abs(closest_floor_down - current_floor) > abs(closest_floor_up-current_floor)){
	  orders_target = closest_floor_up;
	  orders_add_to_target_buffer(orders_target);
	  vector_remove(orders_up, order_up_index);
	  return orders_target;
	}
	else if (abs(closest_floor_down - current_floor) == abs(closest_floor_up-current_floor)){
	  orders_target = closest_floor_up;
	  orders_add_to_target_buffer(orders_target);
	  vector_remove(orders_up, order_up_index); 
	  return orders_target;
	}
      }
    }
  
  return orders_target;
}

void orders_add_order(HardwareOrder hardware_order, int floor){
  switch (hardware_order)
  {
    case HARDWARE_ORDER_UP:
      orders_add_to_orders_up(floor);
      break;
    case HARDWARE_ORDER_DOWN:
      orders_add_to_orders_down(floor);
      break;
    case HARDWARE_ORDER_INSIDE:
      orders_add_to_orders_down(floor);
      orders_add_to_orders_up(floor);
      break;
    default:
      break;
  }
  return;
}

void orders_add_to_target_buffer(int floor){
  int orders_target_buffer_size = vector_size(orders_target_buffer);
  for (int i = 0; i < orders_target_buffer_size; i++)
  {
    //Don't add duplicates
    if (orders_target_buffer[i] == floor)
      return;
  }
  vector_add(&orders_target_buffer, floor);
}


//Updating the list orders_up when the up button is pressed on some floor
void orders_add_to_orders_up(int floor){
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
void orders_add_to_orders_down(int floor){
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
  int orders_target_buffer_size = vector_size(orders_target_buffer);
  for(int i = 0; i < orders_target_buffer_size; i++)
  {
    if (orders_target_buffer[i] == floor)
    {
      vector_remove(orders_target_buffer, i--);
      orders_target_buffer_size--;
    }
  }
  if (orders_target == floor)
  {
    orders_target = -1;
  }
  return;
}


void orders_clear_all(){
  vector_free(orders_up);
  vector_free(orders_down);
  vector_free(orders_target_buffer);
  init_orders(); 
}

// Helper functions
void swap(int *xp, int *yp)  
{  
  int temp = *xp;  
  *xp = *yp;  
  *yp = temp;  
}  

void bubbleSort(int arr[], int n)  
{  
  int i, j;  
  for (i = 0; i < n-1; i++)      

    // Last i elements are already in place  
    for (j = 0; j < n-i-1; j++)  
      if (arr[j] > arr[j+1])  
	swap(&arr[j], &arr[j+1]);  
}  
