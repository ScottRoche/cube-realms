#include "renderer.h"

#include <stdlib.h>

#include "core/logger.h"

#include "instance.h"
#include "devices.h"
#include "swap_chain.h"
#include "graphics_pipeline.h"
#include "framebuffer.h"
#include "command_buffers.h"

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

	Framebuffer **framebuffers;
	uint32_t framebuffer_count;

	CommandPool *command_pool;
	CommandBuffer *command_buffer;
};

static struct Renderer renderer;

/* TODO: Move these somewhere else. These should not be here! */
static VkSemaphore image_available;
static VkSemaphore render_finished;

static uint8_t semaphore_create()
{
	VkResult success;
	VkSemaphoreCreateInfo semaphore_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	success = vkCreateSemaphore(renderer.device->logical_device,
	                            &semaphore_info,
	                            NULL,
	                            &image_available);
	if (success != VK_SUCCESS)
	{
		LOG_ERROR("Failed to create image_available semaphore.");
		return 0;
	}

	success = vkCreateSemaphore(renderer.device->logical_device,
	                            &semaphore_info,
	                            NULL,
	                            &render_finished);
	if (success != VK_SUCCESS)
	{
		LOG_ERROR("Failed to create render_finished semaphore.");
		return 0;
	}

	return 1;
}

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

	renderer.framebuffers =
		malloc(sizeof(Framebuffer*) * renderer.swap_chain->image_count);
	for (uint32_t i = 0; i < renderer.swap_chain->image_count; i++)
	{
		renderer.framebuffers[i] = framebuffer_create(renderer.device,
		                                              renderer.graphics_pipeline,
		                                              &renderer.swap_chain->image_views[i],
		                                              &renderer.swap_chain->extent);

		if (renderer.framebuffers[i] == NULL)
		{
			for (uint32_t j = 0; i <= renderer.framebuffer_count; j++)
			{
				framebuffer_destroy(renderer.framebuffers[j], renderer.device);
			}
			goto framebuffer_create_fail;
		}

		renderer.framebuffer_count++;
	}

	renderer.command_pool = command_pool_create(renderer.device);
	if (renderer.command_pool == NULL)
	{
		goto command_pool_create_fail;
	}

	renderer.command_buffer = command_buffer_create(renderer.command_pool,
	                                                renderer.device,
	                                                renderer.graphics_pipeline,
	                                                renderer.swap_chain,
	                                                renderer.framebuffers);

	if (renderer.command_buffer == NULL)
	{
		goto command_buffer_create_fail;
	}

	if (!semaphore_create())
	{
		goto semaphore_create_fail;
	}

	return 1;

semaphore_create_fail:
	command_buffer_destroy(renderer.command_buffer,
	                       renderer.command_pool,
	                       renderer.device);
	command_pool_destroy(renderer.command_pool, renderer.device);

command_buffer_create_fail:
	command_pool_destroy(renderer.command_pool, renderer.device);

command_pool_create_fail:
	for (uint32_t i = 0; i < renderer.swap_chain->image_count; i++)
	{
		framebuffer_destroy(renderer.framebuffers[i], renderer.device);
	}

framebuffer_create_fail:
	free(renderer.framebuffers);
	graphics_pipeline_destroy(renderer.graphics_pipeline, renderer.device);

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
	vkDestroySemaphore(renderer.device->logical_device, image_available, NULL);
	vkDestroySemaphore(renderer.device->logical_device, render_finished, NULL);

	for (uint32_t i = 0; i < renderer.swap_chain->image_count; i++)
	{
		framebuffer_destroy(renderer.framebuffers[i], renderer.device);
	}
	free(renderer.framebuffers);

	command_buffer_destroy(renderer.command_buffer,
	                       renderer.command_pool,
	                       renderer.device);
	command_pool_destroy(renderer.command_pool, renderer.device);

	graphics_pipeline_destroy(renderer.graphics_pipeline, renderer.device);
	swap_chain_destroy(renderer.swap_chain, renderer.device);
	vkDestroySurfaceKHR(renderer.instance->handle,
	                    renderer.render_surface,
	                    NULL);

	device_destroy(renderer.device);
	instance_destroy(renderer.instance);
}

void renderer_draw()
{
	uint32_t image_index;
	VkResult success;

	/* Submit to queue */
	vkAcquireNextImageKHR(renderer.device->logical_device,
	                      renderer.swap_chain->handle,
	                      UINT64_MAX, /* Disable timeout */
	                      image_available,
	                      VK_NULL_HANDLE,
	                      &image_index);

	if (image_index > renderer.command_buffer->buffer_count)
	{
		LOG_FATAL(2, "Aquired image index is greater than number of command buffers");
	}

	VkSemaphore wait_semaphores[] = {image_available};
	VkSemaphore signal_semaphores[] = {render_finished};
	VkPipelineStageFlags wait_stages[]
		= {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

	VkSubmitInfo submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = wait_semaphores,
		.pWaitDstStageMask = wait_stages,
		.commandBufferCount = 1,
		.pCommandBuffers = &renderer.command_buffer->buffers[image_index],
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signal_semaphores
	};

	success = vkQueueSubmit(renderer.device->graphics_queue,
	                        1,
							&submit_info,
							VK_NULL_HANDLE);
	if (success != VK_SUCCESS)
	{
		LOG_FATAL(3, "Failed to submit queue");
	}

	/* Present */
	VkPresentInfoKHR present_info = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signal_semaphores,
		.swapchainCount = 1,
		.pSwapchains = &renderer.swap_chain->handle,
		.pImageIndices = &image_index,
		.pResults = NULL
	};

	/* We've got to do something about these long accesses. */
	vkQueuePresentKHR(renderer.device->present_queue,
	                  &present_info);

	vkDeviceWaitIdle(renderer.device->logical_device);
}