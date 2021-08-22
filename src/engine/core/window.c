#include "window.h"

#include <stdio.h>
#include <stdlib.h>

#include "logger.h"

Window *window_create()
{
	Window *window = malloc(sizeof(Window));

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); /* Only necessary now because Vulkan renderer not setup for resizing */
	window->handle = glfwCreateWindow(800, 600, "Cube Realms", NULL, NULL);

	if (window->handle == NULL)
	{
		LOG_ERROR("Failed to create GLFWwindow");
		return NULL;
	}

	return window;
}

void window_destroy(Window *restrict window)
{
	glfwDestroyWindow(window->handle);
	free(window);
}

void window_update(Window *restrict window)
{
	while (!glfwWindowShouldClose(window->handle))
	{
		glfwPollEvents();
	}
}