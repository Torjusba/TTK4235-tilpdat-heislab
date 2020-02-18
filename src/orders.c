#include "orders.h"


void init_orders(){
    orders_up = vector_create(); 
    orders_down = vector_create();
    targets = vector_create();
    target_buffer = vector_create();
    target = 0; 
}



int orders_get_new_target(HardwareMovement hardware_movement, int curr_floor){
    if (hardware_movement == HARDWARE_MOVEMENT_UP){
        int orders_up_size = vector_size(orders_up);
        //Going through orders_up to see if we can do some stops on the way
        //And adding them to target

        for (int i = 1; i<= orders_up_size; i++){
            if (orders_up[i] > curr_floor && target > orders_up[i]){
                vector_add(&target_buffer, target);
                target = orders_up[i]; 

            }
        }
    }
    else if (hardware_movement == HARDWARE_MOVEMENT_DOWN){
        int orders_down_size = vector_size(orders_down);
        //Going through orders_down to see if we can do some stops on the way
        //And adding them to targets
        for (int i = 1; i<= orders_down_size; i++){
            if (orders_down[i] > curr_floor && target > orders_down[i]){
                vector_add(&target_buffer, target);
                target = orders_down[i]; 
            }
        }
    }
    //Checking if target is empty, and new target have to be included. 
    else if (target == -1){
        int buffer_size = vector_size(target_buffer);
        if (buffer_size > 0){
            bubbleSort(target_buffer, buffer_size);
            target = target_buffer[1];
            vector_remove(target_buffer,1); 
        }
        else if (buffer_size == 0 && target == -1){
            //Find closest order, and set to target
            int orders_up_size = vector_size(orders_up);
            int orders_down_size = vector_size(orders_down);
            int closest_floor_up = orders_up[1];
            int closest_floor_down = orders_down[1];
            int order_up_index;
            int order_down_index;
            //Find closest floor to the elevator
            for (int i = 2; i <= orders_up_size; i++){
                 if (abs(closest_floor_up - curr_floor) > abs(orders_up[i] - curr_floor)){

                    closest_floor_up = orders_up[i];
                    order_up_index = i;
                }
            }
            for (int i = 2; i <= orders_down_size; i++){
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
    return target; 

}

void orders_add_order(HardwareOrder hardware_order, int floor){
    update_orders_down(hardware_order, floor);
    update_orders_up(hardware_order, floor); 
}



//Updating the list orders_up when the up button is pressed on some floor
void update_orders_up(HardwareOrder hardware_order, int floor){
    if (hardware_order == HARDWARE_ORDER_UP){
        int orders_up_size = vector_size(orders_up);
        vector_add(&orders_up, floor);
        
    }

}

//Updating the list orders_down when the down button is pressed on some floor
void update_orders_down(HardwareOrder hardware_order, int floor){
    if (hardware_order == HARDWARE_ORDER_DOWN){
        int orders_down_size = vector_size(orders_down);
        vector_add(&orders_up, floor);
        
    }

}

void orders_clear_target(int floor){
    target = 0; 
}


void orders_clear_all(){
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