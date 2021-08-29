#ifndef _RENDERER_H_
#define _RENDERER_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/window.h"

/******************************************************************************
 * @name   renderer_init()
 * @brief  Initalises the renderer for use.
 * @return A value describing the success of the initalisation. 1 if successful
 *         0 if failed.
******************************************************************************/
int renderer_init(const Window *window);

/******************************************************************************
 * @name  renderer_deinit()
 * @brief Deinitalises the renderer.
******************************************************************************/
void renderer_deinit();

#endif /* _RENDERER_H_ */