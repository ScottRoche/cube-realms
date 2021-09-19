#include "renderer.h"

#include <stdlib.h>
#include <string.h>

#include "core/logger.h"

#include "instance.h"
#include "devices.h"
#include "swap_chain.h"
#include "graphics_pipeline.h"
#include "framebuffer.h"
#include "command_buffers.h"
#include "buffer.h"

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

	VertexBuffer *vbuffer;
};

static struct Renderer renderer;

/* TODO: Move these somewhere else. These should not be here! */
static VkSemaphore image_available;
static VkSemaphore render_finished;

static ENGINE_ERROR semaphore_create()
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
		ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_OUT_OF_MEMORY,
		                           "Failed to initalise semaphore insufficient"
		                           "host/device memory");
	}

	success = vkCreateSemaphore(renderer.device->logical_device,
	                            &semaphore_info,
	                            NULL,
	                            &render_finished);
	if (success != VK_SUCCESS)
	{
		ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_OUT_OF_MEMORY,
		                           "Failed to initalise semaphore insufficient"
		                           "host/device memory");
	}

	return ENGINE_OK;
}

static ENGINE_ERROR create_framebuffers(Framebuffer **framebuffer_array,
                                        uint32_t image_count)
{
	for (uint32_t i = 0; i < image_count; i++)
	{
		framebuffer_array[i] = framebuffer_create(renderer.device,
		                                          renderer.graphics_pipeline,
		                                          &renderer.swap_chain->image_views[i],
		                                          &renderer.swap_chain->extent);

		if (framebuffer_array[i] == NULL)
		{
			for (uint32_t j = 0; framebuffer_array[j] != NULL; j++)
			{
				framebuffer_destroy(framebuffer_array[j], renderer.device);
			}

			return ENGINE_ERROR_INIT_FAILED;
		}
	}
	return ENGINE_OK;
}

ENGINE_ERROR renderer_init(const Window *window)
{
	ENGINE_ERROR error = ENGINE_OK;
	VkResult surface_status = 0;

	error = instance_create(&renderer.instance);
	ENGINE_LOG_RETURN_IF_ERROR(error, "Vulkan instance failed to initalise");

	/* Should we be doing this here? Maybe not. */
	surface_status = glfwCreateWindowSurface(renderer.instance->handle,
	                                         window->handle,
	                                         NULL,
	                                         &renderer.render_surface);
	if (surface_status != VK_SUCCESS)
	{
		instance_destroy(renderer.instance);
		ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_INIT_FAILED,
		                           "Failed to create Vulkan surface for Window");
	}

	error = device_create(&renderer.device,
	                      renderer.instance,
	                      &renderer.render_surface);

	ENGINE_GOTO_IF_ERROR(error, device_init_fail);

	error = swap_chain_create(&renderer.swap_chain,
	                          window,
	                          renderer.device,
	                          &renderer.render_surface);

	ENGINE_GOTO_IF_ERROR(error, swap_chain_init_fail);

	/* Won't be here in the future */
	float verticies[] = {
		 0.0f, -0.5f, 1.0f, 0.0f, 0.0f,
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
	};

	VertexData *vertex_data =
		vertex_data_create(verticies, sizeof(verticies) / sizeof(float));

	renderer.vbuffer = vertex_buffer_create(renderer.device,
	                                        sizeof(verticies));

	void *data;
	vkMapMemory(renderer.device->logical_device,
	            renderer.vbuffer->buffer_memory,
	            0,
	            sizeof(verticies),
	            0,
	            &data);
	memcpy(data, verticies, sizeof(verticies));
	vkUnmapMemory(renderer.device->logical_device,
	              renderer.vbuffer->buffer_memory);

	error = graphics_pipeline_create(&renderer.graphics_pipeline,
	                                 renderer.device,
	                                 renderer.swap_chain,
	                                 vertex_data);

	ENGINE_GOTO_IF_ERROR(error, graphics_pipeline_init_fail);

	renderer.framebuffers =
		malloc(sizeof(Framebuffer*) * renderer.swap_chain->image_count);

	error = create_framebuffers(renderer.framebuffers,
	                            renderer.swap_chain->image_count);
	ENGINE_LOG_GOTO_IF_ERROR(error,
	                         "Failed to intialise all framebuffers",
	                         framebuffer_init_fail);

	error = command_pool_create(&renderer.command_pool, renderer.device);
	ENGINE_GOTO_IF_ERROR(error, command_pool_init_fail);

	error = command_buffer_create(&renderer.command_buffer,
	                              renderer.command_pool,
	                              renderer.device,
	                              renderer.graphics_pipeline,
	                              renderer.swap_chain,
	                              renderer.framebuffers,
	                              renderer.vbuffer,
	                              sizeof(verticies));

	vertex_data_destroy(vertex_data);

	ENGINE_LOG_GOTO_IF_ERROR(error,
	                         "Failed to initalise command buffer",
	                         command_buffer_init_fail);

	error = semaphore_create();
	ENGINE_GOTO_IF_ERROR(error, semaphore_init_fail);

	return ENGINE_OK;

semaphore_init_fail:
	command_buffer_destroy(renderer.command_buffer,
	                       renderer.command_pool,
	                       renderer.device);
	command_pool_destroy(renderer.command_pool, renderer.device);

command_buffer_init_fail:
	command_pool_destroy(renderer.command_pool, renderer.device);

command_pool_init_fail:
	for (uint32_t i = 0; i < renderer.swap_chain->image_count; i++)
	{
		framebuffer_destroy(renderer.framebuffers[i], renderer.device);
	}

framebuffer_init_fail:
	free(renderer.framebuffers);
	graphics_pipeline_destroy(renderer.graphics_pipeline, renderer.device);

graphics_pipeline_init_fail:
	vertex_data_destroy(vertex_data);
	swap_chain_destroy(renderer.swap_chain, renderer.device);

swap_chain_init_fail:
	device_destroy(renderer.device);

device_init_fail:
	vkDestroySurfaceKHR(renderer.instance->handle,
	                    renderer.render_surface,
	                    NULL);

	return error;
}

void renderer_deinit()
{
	vertex_buffer_destroy(renderer.vbuffer, renderer.device);

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
		LOG_FATAL("Aquired image index is greater than number of command buffers");
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
		LOG_FATAL("Failed to submit queue");
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

	vkQueuePresentKHR(renderer.device->present_queue,
	                  &present_info);

	vkDeviceWaitIdle(renderer.device->logical_device);
}