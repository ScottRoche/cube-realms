#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <GLFW/glfw3.h>

/******************************************************************************
 * @name  _Window
 * @brief A struct that encapsulates a GLFWwindow and contains additional
 *        properties.
******************************************************************************/
struct _Window
{
	GLFWwindow *handle;
};
typedef struct _Window Window;

/******************************************************************************
 * @name   window_create()
 * @brief  Creates and initialises an instance of the Window struct.
 * @return A pointer to a Window struct or NULL if it failed.
******************************************************************************/
Window *window_create();

/******************************************************************************
 * @name      window_destroy()
 * @brief     Destroys a Window by deinitialising and freeing it's memory.
 * @param[in] window A pointer to a window that will be destroyed.
 * @return    void
******************************************************************************/
void window_destroy(Window *restrict window);

/******************************************************************************
 * @name      window_update()
 * @brief     Updates the specified window.
 * @param[in] window A pointer to a window that will be updated.
 * @return    void
******************************************************************************/
void window_update(Window *restrict window);

#endif /* _WINDOW_H_ */