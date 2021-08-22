#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "window.h"

/******************************************************************************
 * @name  _Application
 * @brief An object to hold properties of an application.
******************************************************************************/
struct _Application
{
	Window *window;
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