#include "renderer.h"

#include "core/logger.h"

#include "instance.h"
#include "devices.h"
#include "swap_chain.h"
#include "graphics_pipeline.h"

/******************************************************************************
 * @name Renderer
 * @brief An object to encapsulate properties related to the renderer.
******************************************************************************/
struct Renderer 
{
	Instance *instance;
	Device *device;
	VkSurfaceKHR render_surface;
	SwapChain *swap_chain;
	GraphicsPipeline *graphics_pipeline;
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

	renderer.swap_chain = swap_chain_create(window,
	                                        renderer.device,
	                                        &renderer.render_surface);
	if (renderer.swap_chain == NULL)
	{
		goto swap_chain_create_fail;
	}

	renderer.graphics_pipeline = graphics_pipeline_create(renderer.device,
	                                                      renderer.swap_chain);
	if (renderer.graphics_pipeline == NULL)
	{
		goto graphics_pipeline_create_fail;
	}

	return 1;

graphics_pipeline_create_fail:
	swap_chain_destroy(renderer.swap_chain, renderer.device);

swap_chain_create_fail:
	device_destroy(renderer.device);

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
	graphics_pipeline_destroy(renderer.graphics_pipeline, renderer.device);
	swap_chain_destroy(renderer.swap_chain, renderer.device);
	vkDestroySurfaceKHR(renderer.instance->handle,
	                    renderer.render_surface,
	                    NULL);

	device_destroy(renderer.device);
	instance_destroy(renderer.instance);
}