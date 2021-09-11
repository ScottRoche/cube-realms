#include "command_buffers.h"

#include <stdlib.h>
#include <stdint.h>

#include "core/logger.h"

CommandPool *command_pool_create(Device *device)
{
	CommandPool *pool = NULL;
	VkResult success;

	if (device->queue_family_indicies.graphics_family == -1)
	{
		LOG_ERROR("Graphics queue family invalid");
		return NULL;
	}

	pool = malloc(sizeof(CommandPool));
	VkCommandPoolCreateInfo pool_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = device->queue_family_indicies.graphics_family,
		.flags = 0
	};

	success = vkCreateCommandPool(device->logical_device,
	                              &pool_info,
	                              NULL,
	                              &pool->handle);
	if (success != VK_SUCCESS)
	{
		LOG_ERROR("vkCreateCommandPool failed");
		free(pool);
		return NULL;
	}

	return pool;
}

void command_pool_destroy(CommandPool *pool, Device *device)
{
	vkDestroyCommandPool(device->logical_device, pool->handle, NULL);
	free(pool);
}

CommandBuffer *command_buffer_create(const CommandPool *restrict pool,
                                      const Device *restrict device,
                                      const GraphicsPipeline *restrict pipeline,
                                      SwapChain *restrict swap_chain,
                                      Framebuffer **framebuffer_array,
                                      VertexBuffer *restrict vertex_buffer,
                                      size_t verticies_size)
{
	CommandBuffer *buffer = malloc(sizeof(CommandBuffer));
	VkResult success;

	buffer->buffer_count = swap_chain->image_count;
	buffer->buffers = calloc(buffer->buffer_count, sizeof(VkCommandBuffer*));

	VkCommandBufferAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = pool->handle,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = swap_chain->image_count
	};

	success = vkAllocateCommandBuffers(device->logical_device,
	                                   &alloc_info,
	                                   buffer->buffers);
	if (success != VK_SUCCESS)
	{
		LOG_ERROR("Failed to allocate command buffer memory");
		goto buffer_allocation_fail;
	}

	for (uint32_t i = 0; i < swap_chain->image_count; i++)
	{
		VkCommandBufferBeginInfo buffer_begin_info = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0,
			.pInheritanceInfo = NULL
		};

		success = vkBeginCommandBuffer(buffer->buffers[i], &buffer_begin_info);
		if (success != VK_SUCCESS)
		{
			LOG_ERROR("vkBeginCommandBuffer failed");
			goto command_buffer_begin_fail;
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

		vkCmdBeginRenderPass(buffer->buffers[i],
		                     &render_pass_info,
		                     VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(buffer->buffers[i],
		                  VK_PIPELINE_BIND_POINT_GRAPHICS,
		                  pipeline->handle);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(buffer->buffers[i], 0, 1, (VkBuffer*)vertex_buffer, &offset);

		vkCmdDraw(buffer->buffers[i], verticies_size / sizeof(float), 1, 0, 0);
		vkCmdEndRenderPass(buffer->buffers[i]);

		if (vkEndCommandBuffer(buffer->buffers[i]) != VK_SUCCESS)
		{
			LOG_ERROR("Failed to record command buffer");
			goto command_buffer_begin_fail;
		}
	}

	return buffer;

command_buffer_begin_fail:
	vkFreeCommandBuffers(device->logical_device,
	                     pool->handle,
	                     buffer->buffer_count,
	                     buffer->buffers);

buffer_allocation_fail:
	free(buffer->buffers);
	free(buffer);

	return NULL;
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