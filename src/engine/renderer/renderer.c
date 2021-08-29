#include "renderer.h"

#include "core/logger.h"

#include "instance.h"
#include "devices.h"

/******************************************************************************
 * @name Renderer
 * @brief An object to encapsulate properties related to the renderer.
******************************************************************************/
struct Renderer 
{
	Instance *instance;
	Device *device;
	VkSurfaceKHR render_surface;
};

static struct Renderer renderer;

int renderer_init(const Window *window)
{
	VkResult success;

	renderer.instance = instance_create();
	if (renderer.instance == NULL)
	{
		goto instance_create_fail;
	}

	/* Should we be doing this here? Maybe not. */
	success = glfwCreateWindowSurface(renderer.instance->handle,
	                                  window->handle,
	                                  NULL,
	                                  &renderer.render_surface);
	if (success != VK_SUCCESS)
	{
		LOG_ERROR("[%d] glfwCreateWindowSurface failed", success)
		goto surface_create_fail;
	}

	renderer.device = device_create(renderer.instance, &renderer.render_surface);
	if (renderer.device == NULL)
	{
		goto device_create_fail;
	}


	return 1;

device_create_fail:
	vkDestroySurfaceKHR(renderer.instance->handle,
	                    renderer.render_surface,
	                    NULL);

surface_create_fail:
	instance_destroy(renderer.instance);

instance_create_fail:
	return 0;
}

void renderer_deinit()
{
	vkDestroySurfaceKHR(renderer.instance->handle,
	                    renderer.render_surface,
	                    NULL);

	device_destroy(renderer.device);
	instance_destroy(renderer.instance);
}