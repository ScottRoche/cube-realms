#include "application.h"

#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "logger.h"

#include "renderer/renderer.h"

static Application application;

int application_initialise()
{
	int success = 1;
	
	success = glfwInit();
	if (!success)
	{
		LOG_ERROR("Failed to initalise GLFW");
		goto glfw_init_fail;
	}

	application.window = window_create();
	if (application.window == NULL)
	{
		LOG_ERROR("Failed to create Window");
		goto window_create_fail;
	}

	success = renderer_init(application.window);
	if (!success)
	{
		LOG_ERROR("Failed to initalise the renderer");
		goto renderer_init_fail;
	}

	return 1;

renderer_init_fail:
	window_destroy(application.window);
window_create_fail:
	glfwTerminate();
glfw_init_fail:
	return 0;
}

void application_destroy()
{
	renderer_deinit();

	window_destroy(application.window);
	glfwTerminate();
}

void application_run()
{
	window_update(application.window);
}