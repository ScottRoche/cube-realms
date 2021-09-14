#include "window.h"

#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

static void error_callback(int error, const char *description)
{
	LOG_ERROR("%d - %s", error, description);
}

ENGINE_ERROR window_create(Window **window)
{
	*window = malloc(sizeof(Window));

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); /* Only necessary now because Vulkan renderer not setup for resizing */
	(*window)->handle = glfwCreateWindow(800, 600, "Cube Realms", NULL, NULL);

	if ((*window)->handle == NULL)
	{
		free(window);
		return ENGINE_ERROR_INIT_FAILED;
	}

	glfwSetErrorCallback(&error_callback);

	return ENGINE_OK;
}

void window_destroy(Window *restrict window)
{
	glfwDestroyWindow(window->handle);
	free(window);
}

void window_update(Window *restrict window)
{
	glfwPollEvents();
}