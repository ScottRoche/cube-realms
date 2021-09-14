#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "window.h"
#include "debug.h"

/******************************************************************************
 * @name  _Application
 * @brief Includes properties the application can manipulate and use.
******************************************************************************/
struct _Application
{
	Window *window;
};
typedef struct _Application Application;

/******************************************************************************
 * @name   application_initialise()
 * @brief  Setup the application to be run.
 * @return ENGINE_ERROR value to indicate the success or communicate why the
 *         the function failed.
******************************************************************************/
ENGINE_ERROR application_initialise();

/******************************************************************************
 * @name   application_destroy()
 * @brief  Destroy application memory and stop running the program.
 * @return void
******************************************************************************/
void application_destroy();

/******************************************************************************
 * @name   application_run()
 * @brief  Begin the application loop.
 * @return void
******************************************************************************/
void application_run();

#endif /* _APPLICATION_H_ */