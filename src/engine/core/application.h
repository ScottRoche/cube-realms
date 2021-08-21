#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "window.h"

/******************************************************************************
 * @name  _Application
 * @brief An object to hold properties of an application.
******************************************************************************/
struct _Application
{
	/* An array of Window pointers */
	Window **windows;
	/* The size of the windows array */
	size_t windows_size;
	/* The number of pointers within the array */
	size_t windows_count;
};
typedef struct _Application Application;

/******************************************************************************
 * @name   application_initialise()
 * @brief  Setup the application to be run.
 * @return An integer value describing the success of the initialisation.
******************************************************************************/
int application_initialise();

/******************************************************************************
 * @name   application_destroy()
 * @brief  Deinitialise the application.
 * @return void
******************************************************************************/
void application_destroy();

/******************************************************************************
 * @name   application_run()
 * @brief  Begin the main loop of the application.
 * @return void
******************************************************************************/
void application_run();

#endif /* _APPLICATION_H_ */