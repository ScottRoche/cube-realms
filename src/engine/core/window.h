#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GLFW/glfw3.h>

#include "debug.h"

/******************************************************************************
 * @name  _Window
 * @brief Includes properties of the a window such as a GLFWwindow handle.
******************************************************************************/
struct _Window
{
	GLFWwindow *handle; /*< The GLFWwindow instance that this Window handles */
};
typedef struct _Window Window;

/******************************************************************************
 * @name       window_create()
 * @brief      Creates and initialises an instance of the Window struct.
 * @param[out] window A pointer to a pointer which will store the address of the
 *                    created Window struct in memory.
 * @return     ENGINE_ERROR value to indicate the success or communicate why the
 *             the function failed.
******************************************************************************/
ENGINE_ERROR window_create(Window **window);

/******************************************************************************
 * @name      window_destroy()
 * @brief     Destroys a Window.
 * @param[in] window A pointer to a window that will be destroyed.
 * @return    void
******************************************************************************/
void window_destroy(Window *restrict window);

/******************************************************************************
 * @name      window_update()
 * @brief     A function to update the state of a window updates a specified
 *            window.
 * @param[in] window A pointer to a window to be updated.
 * @return    void
******************************************************************************/
void window_update(Window *restrict window);

#endif /* _WINDOW_H_ */