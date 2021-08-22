#include "application.h"

#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "logger.h"

static Application application;

int application_initialise()
{
	int success = glfwInit();

	if (!success)
	{
		LOG_ERROR("Failed to initalise GLFW");
		return 0;
	}

	application.window = window_create();
	if (application.window == NULL)
	{
		LOG_ERROR("Failed to create Window");
		return 0;
	}
	
	LOG_DEBUG("Applicaiton initalised");
	return 1;
}

void application_destroy()
{
	window_destroy(application.window);
	glfwTerminate();
	LOG_DEBUG("Application Destroyed");
}

void application_run()
{
	LOG_DEBUG("Application Running");
	window_update(application.window);
}