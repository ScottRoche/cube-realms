#include "application.h"

#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "logger.h"

#include "renderer/renderer.h"

static Application application = { NULL };

ENGINE_ERROR application_initialise()
{
	ENGINE_ERROR error;
	
	if (!glfwInit())
	{
		ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_INIT_FAILED,
		                           "GLFW failed to initalise");
	}

	error = window_create(&application.window);
	ENGINE_LOG_GOTO_IF_ERROR(error,
	                         "Window failed to be initalised",
	                         window_create_fail);

	error = renderer_init(application.window);
	ENGINE_GOTO_IF_ERROR(error, renderer_init_fail)

	return ENGINE_OK;

renderer_init_fail:
	window_destroy(application.window);
window_create_fail:
	glfwTerminate();
	return error;
}

void application_destroy()
{
	renderer_deinit();

	window_destroy(application.window);
	glfwTerminate();
}

void application_run()
{
	while (!glfwWindowShouldClose(application.window->handle))
	{
		window_update(application.window);
		renderer_draw();
	}
}