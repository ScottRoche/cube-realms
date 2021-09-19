#include "command_buffers.h"

#include <stdlib.h>
#include <stdint.h>

#include "core/logger.h"

ENGINE_ERROR command_pool_create(CommandPool **command_pool, Device *device)
{
	VkResult success;

	if (device->queue_family_indicies.graphics_family == -1)
	{
		ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_INIT_FAILED,
		                           "Graphics queue family is invalid.");
	}

	*command_pool = malloc(sizeof(CommandPool));
	VkCommandPoolCreateInfo pool_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = device->queue_family_indicies.graphics_family,
		.flags = 0
	};

	success = vkCreateCommandPool(device->logical_device,
	                              &pool_info,
	                              NULL,
	                              &(*command_pool)->handle);
	if (success == VK_ERROR_OUT_OF_HOST_MEMORY
	    || success == VK_ERROR_OUT_OF_DEVICE_MEMORY)
	{
		
		free(*command_pool);
		command_pool = NULL;

		ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_OUT_OF_MEMORY,
		                           "Failed to create command buffer, insufficent"
		                           "host/device memory")
		
	}

	return ENGINE_OK;
}

void command_pool_destroy(CommandPool *pool, Device *device)
{
	vkDestroyCommandPool(device->logical_device, pool->handle, NULL);
	free(pool);
}

ENGINE_ERROR command_buffer_create(CommandBuffer **command_buffer,
                                   const CommandPool *restrict pool,
                                   const Device *restrict device,
                                   const GraphicsPipeline *restrict pipeline,
                                   SwapChain *restrict swap_chain,
                                   Framebuffer **framebuffer_array,
                                   VertexBuffer *restrict vertex_buffer,
                                   size_t verticies_size)
{
	*command_buffer = malloc(sizeof(CommandBuffer));
	ENGINE_ERROR error;
	VkResult success;

	(*command_buffer)->buffer_count = swap_chain->image_count;
	(*command_buffer)->buffers = calloc((*command_buffer)->buffer_count, sizeof(VkCommandBuffer*));

	VkCommandBufferAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = pool->handle,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = swap_chain->image_count
	};

	success = vkAllocateCommandBuffers(device->logical_device,
	                                   &alloc_info,
	                                   (*command_buffer)->buffers);
	if (success != VK_SUCCESS)
	{
		error = ENGINE_ERROR_OUT_OF_MEMORY;
		ENGINE_LOG_GOTO_IF_ERROR(error,
		                         "Failed to allocate command buffer memory",
		                         buffer_allocation_fail);
	}

	for (uint32_t i = 0; i < swap_chain->image_count; i++)
	{
		VkCommandBufferBeginInfo buffer_begin_info = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0,
			.pInheritanceInfo = NULL
		};

		success = vkBeginCommandBuffer((*command_buffer)->buffers[i], &buffer_begin_info);
		if (success != VK_SUCCESS)
		{
			error = ENGINE_ERROR_OUT_OF_MEMORY;
			ENGINE_LOG_GOTO_IF_ERROR(error,
			                         "Failed to begin command buffer, insufficient memory",
			                         command_buffer_record_fail);
		}

		VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

		VkRenderPassBeginInfo render_pass_info = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = pipeline->render_pass,
			.framebuffer = framebuffer_array[i]->handle,
			.renderArea.offset = {0, 0},
			.renderArea.extent = swap_chain->extent,
			.clearValueCount = 1,
			.pClearValues = &clear_color
		};

		vkCmdBeginRenderPass((*command_buffer)->buffers[i],
		                     &render_pass_info,
		                     VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline((*command_buffer)->buffers[i],
		                  VK_PIPELINE_BIND_POINT_GRAPHICS,
		                  pipeline->handle);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers((*command_buffer)->buffers[i], 0, 1, (VkBuffer*)vertex_buffer, &offset);

		vkCmdDraw((*command_buffer)->buffers[i], verticies_size / sizeof(float), 1, 0, 0);
		vkCmdEndRenderPass((*command_buffer)->buffers[i]);

		if (vkEndCommandBuffer((*command_buffer)->buffers[i]) != VK_SUCCESS)
		{
			error = ENGINE_ERROR_OUT_OF_MEMORY;
			ENGINE_LOG_GOTO_IF_ERROR(error,
			                         "Failed to record command buffer, insufficient memory",
									 command_buffer_record_fail);
		}
	}

	return ENGINE_OK;

command_buffer_record_fail:
	vkFreeCommandBuffers(device->logical_device,
	                     pool->handle,
	                     (*command_buffer)->buffer_count,
	                     (*command_buffer)->buffers);

buffer_allocation_fail:
	free((*command_buffer)->buffers);
	free((*command_buffer));
	*command_buffer = NULL;
	return error;
}

void command_buffer_destroy(CommandBuffer *buffer,
                            CommandPool *pool,
                            Device *device)
{
	vkFreeCommandBuffers(device->logical_device,
	                     pool->handle,
	                     buffer->buffer_count,
	                     buffer->buffers);
	free(buffer->buffers);
	free(buffer);
}