// Application file to Demonstrate the Usage of Timer Manager
// Include Header Files
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "TypeDefines.h"
#include "TimerMgrHeader.h"
#include "TimerAPI.h"

// Function to Print the Time
void print_time(void)
{

}

// Function to Print the Time and Msg in Callback Function
void print_time_msg(int num)
{

	// Function to print the message for each Timer like
	// "This is Function 2 and UTC time and date: Thu Oct 27 20:53:27 2016
}

void print_program_info(void)
{
	fprintf(stdout, "\n\n\n\nTimer Manager Project");
	fprintf(stdout, "\n=====================");

	fprintf(stdout, "\n\nCreated by: Ahsan Shahab");

	fprintf(stdout, "\n\n-> This Program will initialize the timers and creates Timer Task as Thread");
	fprintf(stdout, "\n-> It creates 3 Timers");
	fprintf(stdout, "\n\tTimer1 - Periodic 5 second");
	fprintf(stdout, "\n\tTimer2 - Periodic 3 second");
	fprintf(stdout, "\n\tTimer3 - One Shot 10 second");
	fprintf(stdout, "\n\nPress Enter to start the Program...\n");

	getchar();
}

void function1(void *arg)
{
	print_time_msg(1);
}

void function2(void *arg)
{
	print_time_msg(2);
}

void function3(void *arg)
{
	print_time_msg(3);
}

/*To test your project, you need to write a program that prints the current time when timers are created. Then it creates 3 timers that register handles “function1”, “function2” and “function3”, respectively.
Timer 1 gets invoked every 5 seconds and runs function1 which prints “this is is function 1 and time is = <print current time>
Timer 2 gets invoked every 3 seconds and runs function1 which prints “this is is function 2 and time is = <print current time>
Timer 3 gets invoked only once 10 seconds after it was created and runs function3 which prints “this is is function 3 and time is = <print current time>
*/
int main(void) 
{
	INT8U err_val = RTOS_ERR_NONE;

	RTOS_TMR *timer_obj1 = NULL;
	RTOS_TMR *timer_obj2 = NULL;
	RTOS_TMR *timer_obj3 = NULL;

	INT8 *timer_name[3] = {"Timer1", "Timer2", "Timer3"};

	// Display the Program Info
	print_program_info();

	// Initialize the OS Tick
	OSTickInitialize();

	fprintf(stdout, "OS Tick Initialization completed successfully");

	// Initialize the RTOS Timer
	RTOSTmrInit();

	fprintf(stdout, "\nApplication Started....... :-)\n");

	// ================================================================
	// Timer Creation
	// ================================================================
	
	// Create Timer1
	// Provide the required arguments in the function call
	fprintf(stdout, "\ncheckplz\n");
	timer_obj1 = RTOSTmrCreate(5, 5, RTOS_TMR_PERIODIC, &function1, NULL, timer_name[0], &err_val);
	fprintf(stdout, "checkidfk");

	// Check the return value and determine if it created successfully or not
	
	// Create Timer2
	// Provide the required arguments in the function call
	// timer_obj2 = RTOSTmrCreate(3, 3, RTOS_TMR_PERIODIC, function2, NULL, timer_name[1], &err_val);
	// // Check the return value and determine if it created successfully or not

	// // Create Timer3
	// // Provide the required arguments in the function call
	// timer_obj3 = RTOSTmrCreate(10, 0, RTOS_TMR_ONE_SHOT, function3, NULL, timer_name[2], &err_val);
	// Check the return value and determine if it created successfully or not

	// ================================================================
	// Starting Timer
	// ================================================================
	// Start Timer1
	RTOSTmrStart(timer_obj1, &err_val);
	// Check the return value and determine if it started successfully or not

	// Start Timer2
	// RTOSTmrStart(timer_obj2, &err_val);
	// // Check the return value and determine if it started successfully or not

	// // Start Timer3
	// RTOSTmrStart(timer_obj3, &err_val);
	// Check the return value and determine if it started successfully or not
	// fprintf(stdout, "CHECKING");

	// Other Code if needed	

	return 0;
}

