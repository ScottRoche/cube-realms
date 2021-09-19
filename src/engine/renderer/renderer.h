#ifndef _RENDERER_H_
#define _RENDERER_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/window.h"
#include "core/debug.h"

/******************************************************************************
 * @name   renderer_init()
 * @brief  Initalises the renderer for use.
 * @return A ENGINE_ERROR value to display the status of the initalisation. If
 *         successful ENGINE_OK.
******************************************************************************/
ENGINE_ERROR renderer_init(const Window *window);

/******************************************************************************
 * @name  renderer_deinit()
 * @brief Deinitalises the renderer.
******************************************************************************/
void renderer_deinit();

/******************************************************************************
 * @name  renderer_draw()
 * @brief Renderer a frame.
******************************************************************************/
void renderer_draw();

#endif /* _RENDERER_H_ */