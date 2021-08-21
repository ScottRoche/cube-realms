#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <stdio.h>

/******************************************************************************
 * @name  _Window
 * @brief A struct that encapsulates a GLFWwindow and contains additional
 *        properties.
******************************************************************************/
struct _Window
{

};
typedef struct _Window Window;

/******************************************************************************
 * @name   window_create()
 * @brief  Creates and initialises an instance of the Window struct.
 * @return A pointer to a Window struct or NULL if it failed.
******************************************************************************/
Window *window_create();

/******************************************************************************
 * @name      window_create()
 * @brief     Destroys a Window by deinitialising and freeing it's memory.
 * @param[in] window A pointer to a window that will be destroyed.
 * @return    void
******************************************************************************/
void window_destroy(const Window *window);

#endif /* _WINDOW_H_ */