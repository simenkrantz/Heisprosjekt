#include "elev.h"
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

// Own header files
#include "ownfuncs.h"


// Number of buttons for each floor
#define N_BUTTONS 3

int main()
{

    // Initialize hardware
    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }

    /**
    @var order_list: Elements in the array are set to 1 if the associated button is pressed
	[ [UP from 1st, 	0		 , to 1st],
	[  UP from 2nd, DOWN from 2nd, to 2nd],
	[  UP from 3rd, DOWN from 3rd, to 3rd],
	[  		0	  , DOWN from 4th, to 4th]]

	@var motor_direction is last direction, is -1 if DIRN_DOWN, 1 if DIRN_UP
    */
    int order_list[N_FLOORS][N_BUTTONS] = {{0},{0},{0}};


	// Spec. 4.1 "Oppstart"

    printf("Press STOP button to stop elevator and exit program.\n");
    elev_set_motor_direction(DIRN_UP);
    int *motor_direction = &(int){1};


    while (1) {
        if (elev_get_floor_sensor_signal() != -1) {
            elev_set_motor_direction(DIRN_STOP);
            break;
        }
    }

    int last_passed_floor = elev_get_floor_sensor_signal();
    int order_floor = -1;
    bool order_found = false;


    // MAIN LOOP       Spec. 4.1 -- 4.7

	while (1) {
        set_floor_lights();

//        printf("%s", "Main loop\n");

        if(elev_get_floor_sensor_signal() != -1) {

        	// THIS IS TROUBLE SOMETIMES ! WHY ?
            last_passed_floor = elev_get_floor_sensor_signal();
        }
        
        // STOP STATE          Spec. 4.6
        if (elev_get_stop_signal()) {
            elev_set_stop_lamp(1);
            elev_set_motor_direction(DIRN_STOP);

            while(elev_get_stop_signal()) {
                continue;
            }

            elev_set_stop_lamp(0);

            // Erase orders
            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 3; j++) {
                    order_list[i][j] = 0;
                }
            }
            
            // Checks if array has order
            while(1) {
                for(int floor = 0; floor < 4; floor++) {
                    if(check_up_down_button_pressed(order_list, floor) != 0)
                        break;
                }
            }

            for(int i = 0; i < 4; i++) {
               for(int j = 0; j < 3; j++) {
                   printf("%d", order_list[i][j]);
                }
                printf("\n");
            }
            return -1;

        }


        // IDLE STATE
        do {
            // Check if buttons are pressed and sets lights
            set_order_list_and_lights(order_list);


            // THIS LOGIC DOESNT WORK
            if(elev_get_floor_sensor_signal() != -1 && order_found == false) {
            	elev_set_motor_direction(DIRN_STOP);
            }


            for(int i = 0; i < 4; i++) {
                for(int j = 0; j < 3; j++) {
                    if(order_list[i][j] == 1){
                        order_found = true;
                        order_floor = i;

                        printf("Order floor is %d\n", order_floor);

                        break;
                    }
                }
                if(order_found) {
                    break;
                }
            }
        }while(!order_found);

/**
        for(int i = 0; i < 4; i++) {
               for(int j = 0; j < 3; j++) {
                   printf("%d", order_list[i][j]);
                }
                printf("\n");
            }


        printf("%d", last_passed_floor);
*/

	   	// ORDER STATE
        // NEED STOP

        // Logic to stop in 1st and 4th
        // When ordering from 1st/4th, it goes in an infinte loop!
        // Ordering to 1st/4th is OK 


        while(last_passed_floor != order_floor) {
        	
        	set_order_list_and_lights(order_list);

	        if(order_floor > last_passed_floor) {
	            elev_set_motor_direction(DIRN_UP);
	            *motor_direction = 1;
	        }
	        else if(order_floor < last_passed_floor) {
	        	elev_set_motor_direction(DIRN_DOWN);
	        	*motor_direction = -1;
	        }

	        int index = -1;
	        if(*motor_direction == 1) {
	        	index = 0;
	        }
	        else if(*motor_direction == -1) {
	        	index = 1;
	        }

	        if(elev_get_floor_sensor_signal() != -1) {

	        	// THIS IS TROUBLE SOMETIMES! WHY ?
	            last_passed_floor = elev_get_floor_sensor_signal();

	            //Specifically checking UP button 1st floor, DOWN button 4th floor
	            if(last_passed_floor == 0) {
	            	index = 0;
	            }
	            else if(last_passed_floor == 3) {
	            	index = 1;
	            }


	            printf("%d, %s", last_passed_floor, "Inside order state\n");
	        }


	        set_floor_lights();

	        
	        if((order_list[last_passed_floor][index] == 1) || (order_list[last_passed_floor][2] == 1)
	        	) {
                elev_set_motor_direction(DIRN_STOP);


                // DELETE ONLY LIGHTS CORRESPONDING TO DELETED ORDER_LIST ELEMENT
                turn_off_lights(last_passed_floor, motor_direction);

                open_close_door();      // Cannot update order_list here by now ? Necessary ?
                
                order_list[last_passed_floor][index] = 0;
                order_list[last_passed_floor][2] = 0;



                order_found = false;


                for(int i = 0; i < 4; i++) {
	               for(int j = 0; j < 3; j++) {
	                   printf("%d", order_list[i][j]);
	                }
	                printf("\n");
	            }

            }
	    }


    // End of while loop
    }
  
    return 0;
}
