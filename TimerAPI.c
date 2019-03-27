// Header Files
#include "TypeDefines.h"
#include "TimerMgrHeader.h"
#include "TimerAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>

/*****************************************************
 * Global Variables
 *****************************************************
 */
// Timer Pool Global Variables
INT8U FreeTmrCount = 0;
RTOS_TMR *FreeTmrListPtr = NULL;

// Tick Counter
INT32U RTOSTmrTickCtr = 0;

// Hash Table
HASH_OBJ hash_table[HASH_TABLE_SIZE];

// Thread variable for Timer Task
pthread_t thread;

// Semaphore for Signaling the Timer Task
sem_t timer_task_sem;

// Mutex for Protecting Hash Table
pthread_mutex_t hash_table_mutex;

// Mutex for Protecting Timer Pool
pthread_mutex_t timer_pool_mutex;

/*****************************************************
 * Timer API Functions
 *****************************************************
 */
// Function to create a Timer
RTOS_TMR* RTOSTmrCreate(INT32U delay, INT32U period, INT8U option, RTOS_TMR_CALLBACK callback, void *callback_arg, INT8	*name, INT8U *err)
{
	fprintf(stdout, "check2\n");

	RTOS_TMR *timer_obj = NULL;
	fprintf(stdout, "check3\n");

	// Check the input Arguments for ERROR
	if (delay < 1){
		*err = RTOS_ERR_TMR_INVALID_DLY;
		return NULL;
	}
	fprintf(stdout, "check4\n");

	if (option == RTOS_TMR_PERIODIC && period < 1){
		*err = RTOS_ERR_TMR_INVALID_PERIOD;
		return NULL;
	}
	fprintf(stdout, "check5\n");

	if (option != RTOS_TMR_PERIODIC && option != RTOS_TMR_ONE_SHOT){
		*err = RTOS_ERR_TMR_INVALID_OPT;
		return NULL;
	}
	fprintf(stdout, "check6\n");

	// Allocate a New Timer Obj
	timer_obj = alloc_timer_obj();
		
	fprintf(stdout, "check7\n");

	if(timer_obj == NULL) {
		// Timers are not available
		*err = RTOS_MALLOC_ERR;
		return NULL;
	}
	fprintf(stdout, "check8\n");

	// Fill up the Timer Object
	timer_obj->RTOSTmrCallback = callback;
	timer_obj->RTOSTmrCallbackArg = callback_arg;
	timer_obj->RTOSTmrNext = NULL;
	timer_obj->RTOSTmrPrev = NULL;
	timer_obj->RTOSTmrMatch = delay + RTOSTmrTickCtr;
	timer_obj->RTOSTmrDelay = delay;
	timer_obj->RTOSTmrPeriod = period;
	timer_obj->RTOSTmrName = name;
	timer_obj->RTOSTmrOpt = option;
	timer_obj->RTOSTmrState = RTOS_TMR_STATE_STOPPED;
	
	fprintf(stdout, "check9\n");

	*err = RTOS_SUCCESS;
	fprintf(stdout, "check10\n");

	return timer_obj;
}

// Function to Delete a Timer
INT8U RTOSTmrDel(RTOS_TMR *ptmr, INT8U *perr)
{
	// ERROR Checking

	// Free Timer Object according to its State


	*perr = RTOS_SUCCESS;
	return RTOS_TRUE;
}

// Function to get the Name of a Timer
INT8* RTOSTmrNameGet(RTOS_TMR *ptmr, INT8U *perr)
{
	// ERROR Checking

	// Return the Pointer to the String
}

// To Get the Number of ticks remaining in time out
INT32U RTOSTmrRemainGet(RTOS_TMR *ptmr, INT8U *perr)
{
	// ERROR Checking

	// Return the remaining ticks

}

// To Get the state of the Timer
INT8U RTOSTmrStateGet(RTOS_TMR *ptmr, INT8U *perr)
{
	// ERROR Checking

	// Return State
}

// Function to start a Timer
INT8U RTOSTmrStart(RTOS_TMR *ptmr, INT8U *perr)
{
	// ERROR Checking
	if (ptmr->RTOSTmrType != RTOS_TMR_TYPE){
		*perr = RTOS_ERR_TMR_INVALID_TYPE;
		return *perr;
	}
	// Based on the Timer State, update the RTOSTmrMatch using RTOSTmrTickCtr, RTOSTmrDelay and RTOSTmrPeriod
	// You may use the Hash Table to Insert the Running Timer Obj
	if (ptmr->RTOSTmrState == RTOS_TMR_STATE_STOPPED){
		ptmr->RTOSTmrMatch = RTOSTmrTickCtr + ptmr->RTOSTmrDelay;
	}
	if (ptmr->RTOSTmrState == RTOS_TMR_STATE_COMPLETED && ptmr->RTOSTmrOpt == RTOS_TMR_PERIODIC){
		ptmr->RTOSTmrDelay = ptmr->RTOSTmrPeriod;
		ptmr->RTOSTmrMatch = RTOSTmrTickCtr + ptmr->RTOSTmrDelay;
	}
	ptmr->RTOSTmrState = RTOS_TMR_STATE_RUNNING;
	insert_hash_entry(ptmr);

	return *perr;
}

// Function to Stop the Timer
INT8U RTOSTmrStop(RTOS_TMR *ptmr, INT8U opt, void *callback_arg, INT8U *perr)
{
	// ERROR Checking


	// Remove the Timer from the Hash Table List


	// Change the State to Stopped

	// Call the Callback function if required

}

// Function called when OS Tick Interrupt Occurs which will signal the RTOSTmrTask() to update the Timers
void RTOSTmrSignal(void)
{
	// Received the OS Tick
	// Send the Signal to Timer Task using the Semaphore
	sem_post(&timer_task_sem);

}

/*****************************************************
 * Internal Functions
 *****************************************************
 */

// Create Pool of Timers
INT8U Create_Timer_Pool(INT32U timer_count)
{
	// Create the Timer pool using Dynamic Memory Allocation
	// You can imagine of LinkedList Creation for Timer Obj
	FreeTmrListPtr = (RTOS_TMR*)malloc(timer_count * sizeof(RTOS_TMR*));
	FreeTmrCount = timer_count;
	return RTOS_SUCCESS;
}

// Initialize the Hash Table
void init_hash_table(void)
{
	for (int i = 0; i < HASH_TABLE_SIZE; i++){
		hash_table[i].timer_count = 0;
		hash_table[i].list_ptr = NULL;
	}
}

// Insert a Timer Object in the Hash Table
void insert_hash_entry(RTOS_TMR *timer_obj)
{
	// Calculate the index using Hash Function
	int index = (timer_obj->RTOSTmrDelay + RTOSTmrTickCtr) % HASH_TABLE_SIZE;

	// Lock the Resources
	pthread_mutex_lock(&hash_table_mutex);

	// Add the Entry
	if (hash_table[index].list_ptr == NULL){
		hash_table[index].list_ptr = timer_obj;
	}
	else{
		RTOS_TMR *temp = hash_table[index].list_ptr;
		timer_obj->RTOSTmrNext = temp;
		temp->RTOSTmrPrev = timer_obj;
		hash_table[index].list_ptr = timer_obj;
	}

	// Unlock the Resources
	pthread_mutex_unlock(&hash_table_mutex);

}

// Remove the Timer Object entry from the Hash Table
void remove_hash_entry(RTOS_TMR *timer_obj)
{
	// Calculate the index using Hash Function
	int index = (timer_obj->RTOSTmrDelay + RTOSTmrTickCtr) % HASH_TABLE_SIZE;

	// Lock the Resources
	pthread_mutex_lock(&hash_table_mutex);

	// Remove the Timer Obj
	if (timer_obj->RTOSTmrPrev == NULL){
		hash_table[index].list_ptr = timer_obj->RTOSTmrNext;
		timer_obj->RTOSTmrNext == NULL;
	}
	else{
		timer_obj->RTOSTmrPrev->RTOSTmrNext = timer_obj->RTOSTmrNext;
		timer_obj->RTOSTmrNext == NULL;
		timer_obj->RTOSTmrPrev == NULL;
	}

	// Unlock the Resources
	pthread_mutex_unlock(&hash_table_mutex);

}

// Timer Task to Manage the Running Timers
void *RTOSTmrTask(void *temp)
{

	while(1) {
		// Wait for the signal from RTOSTmrSignal()
		sem_wait(&timer_task_sem);

		// Once got the signal, Increment the Timer Tick Counter
		RTOSTmrTickCtr += 1;

		// Check the whole List associated with the index of the Hash Table
		int index = RTOSTmrTickCtr % HASH_TABLE_SIZE;

		// Compare each obj of linked list for Timer Completion
		// If the Timer is completed, Call its Callback Function, Remove the entry from Hash table
		// If the Timer is Periodic then again insert it in the hash table
		// Change the State
		if (hash_table[index].list_ptr != NULL){
			RTOS_TMR *temp = hash_table[index].list_ptr;
			while (temp != NULL){
				if (temp->RTOSTmrMatch == RTOSTmrTickCtr){
					temp->RTOSTmrState = RTOS_TMR_STATE_COMPLETED;
					temp->RTOSTmrCallback(temp->RTOSTmrCallbackArg);
					remove_hash_entry(temp);
					if (temp->RTOSTmrOpt == RTOS_TMR_PERIODIC){
						INT8U err_val = RTOS_ERR_NONE;
						RTOSTmrStart(temp, &err_val);
					}
				}
			}
		}		
	}
	return temp;
}

// Timer Initialization Function
void RTOSTmrInit(void)
{
	INT32U timer_count = 0;
	INT8U	retVal;
	pthread_attr_t attr;

	fprintf(stdout,"\n\nPlease Enter the number of Timers required in the Pool for the OS ");
	scanf("%d", &timer_count);

	// Create Timer Pool
	retVal = Create_Timer_Pool(timer_count);

	// Check the return Value
	if (retVal != RTOS_SUCCESS){
		fprintf(stdout, "ERROR");
	}
	// Init Hash Table
	init_hash_table();

	fprintf(stdout, "\n\nHash Table Initialized Successfully\n");

	// Initialize Semaphore for Timer Task
	retVal = sem_init(&timer_task_sem, 0, 1);
	if (retVal != RTOS_SUCCESS){
		fprintf(stdout, "ERROR");
	}
	else{
		fprintf(stdout, "\nSemaphore initialized\n");
	}
	
	// Initialize Mutex if any
	retVal = pthread_mutex_init(&hash_table_mutex, NULL);
	retVal = retVal + pthread_mutex_init(&timer_pool_mutex, NULL);
	if (retVal != RTOS_SUCCESS){
		fprintf(stdout, "ERROR");
	}
	else{
		fprintf(stdout,"\nMutexes initialized\n");
	}
	
	// Create any Thread if required for Timer Task
	pthread_attr_init(&attr);
	retVal = pthread_create(&thread, &attr, RTOSTmrTask, NULL);
	if (retVal != RTOS_SUCCESS){
		fprintf(stdout, "ERROR");
	}
	else{
		fprintf(stdout,"\nRTOS Initialization Done...\n");
	}

}

// Allocate a timer object from free timer pool
RTOS_TMR* alloc_timer_obj(void)
{

	// Lock the Resources
	pthread_mutex_lock(&timer_pool_mutex);
	// Check for Availability of Timers
	if (FreeTmrCount > 0){
		FreeTmrCount -= 1;
		// Assign the Timer Object
		RTOS_TMR *temp = (FreeTmrListPtr + FreeTmrCount);
		temp->RTOSTmrType = RTOS_TMR_TYPE;

		pthread_mutex_unlock(&timer_pool_mutex);
		return temp;

	}
	// Unlock the Resources
	pthread_mutex_unlock(&timer_pool_mutex);

	return NULL;
}

// Free the allocated timer object and put it back into free pool
void free_timer_obj(RTOS_TMR *ptmr)
{
	// Lock the Resources

	// Clear the Timer Fields

	// Change the State

	// Return the Timer to Free Timer Pool

	// Unlock the Resources
}

// Function to Setup the Timer of Linux which will provide the Clock Tick Interrupt to the Timer Manager Module
void OSTickInitialize(void) {	
	timer_t timer_id;
	struct itimerspec time_value;

	// Setup the time of the OS Tick as 100 ms after 3 sec of Initial Delay
	time_value.it_interval.tv_sec = 0;
	time_value.it_interval.tv_nsec = RTOS_CFG_TMR_TASK_RATE;

	time_value.it_value.tv_sec = 0;
	time_value.it_value.tv_nsec = RTOS_CFG_TMR_TASK_RATE;

	// Change the Action of SIGALRM to call a function RTOSTmrSignal()
	signal(SIGALRM, &RTOSTmrSignal);

	// Create the Timer Object
	timer_create(CLOCK_REALTIME, NULL, &timer_id);

	// Start the Timer
	timer_settime(timer_id, 0, &time_value, NULL);
}

