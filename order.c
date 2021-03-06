#include <stdio.h>

#include "elev.h"

#include "order.h"
#include "controller.h"


int
get_button_index(int* motor_dir, int array[4][3], int last_floor)
{
	int value = -1;

	if(*motor_dir == 1){
	    for(int i = last_floor + 1; i < 4; i++) {
	    	for(int j = 0; j < 3; j++) {
	    		if(array[i][j] == 1)
	    			value = j;
	   		}
	   	}
	}
	
	else if(*motor_dir == -1){
		for(int i = last_floor - 1; i >= 0; i--) {
	    	for(int j = 0; j < 3; j++) {
	    		if(array[i][j] == 1)
	    			value = j;
       		}
       	}
    }
	return value;
}


void
set_order_matrix_and_corresponding_lights(int array[4][3])
{
	for(int i = 0; i < 4; i++) {
		if(i == 0) {
			if(elev_get_button_signal(BUTTON_CALL_UP, i)) {
				array[i][0] = 1;
				elev_set_button_lamp(BUTTON_CALL_UP, i, 1);
			}
			else if(elev_get_button_signal(BUTTON_COMMAND, i)) {
				array[i][2] = 1;
				elev_set_button_lamp(BUTTON_COMMAND, i, 1);
			}
		}
		else if(i == 3) {
			if(elev_get_button_signal(BUTTON_CALL_DOWN, i)){
				array[i][1] = 1;
				elev_set_button_lamp(BUTTON_CALL_DOWN, i, 1);
			}
			else if(elev_get_button_signal(BUTTON_COMMAND, i)){
				array[i][2] = 1;
				elev_set_button_lamp(BUTTON_COMMAND, i, 1);
			}
		}
		else {
			if(elev_get_button_signal(BUTTON_CALL_UP, i)) {
				array[i][0] = 1;
				elev_set_button_lamp(BUTTON_CALL_UP, i, 1);
			}
			else if(elev_get_button_signal(BUTTON_CALL_DOWN, i)) {
				array[i][1] = 1;
				elev_set_button_lamp(BUTTON_CALL_DOWN, i, 1);
			}
			else if(elev_get_button_signal(BUTTON_COMMAND, i)) {
				array[i][2] = 1;
				elev_set_button_lamp(BUTTON_COMMAND, i, 1);
			}
		}
	}
}


void
stop_handling_at_order_floor(int array[4][3], int last_floor)
{
    elev_set_motor_direction(DIRN_STOP);               
    open_close_door(array);
    for(int i = 0; i < 3; i++){
       	array[last_floor][i] = 0;
    }
    turn_off_button_lights(last_floor);
}


int
search_after_order_in_matrix(int array[4][3]) 
{
	int return_value = -1;
	set_order_matrix_and_corresponding_lights(array);
	for(int i = 0; i < 4; i++) {
	   	for(int j = 0; j < 3; j++) {
	       	if(array[i][j] == 1){
	          	return_value = i;
	        }
	    }
	}
	return return_value;
}
