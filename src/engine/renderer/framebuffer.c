#include "framebuffer.h"

#include <stdlib.h>

#include "core/logger.h"

Framebuffer *framebuffer_create(const Device *restrict device,
                                const GraphicsPipeline *restrict pipeline,
                                const VkImageView *restrict image_view,
                                const VkExtent2D *restrict extent)
{
	Framebuffer *framebuffer = malloc(sizeof(Framebuffer));
	VkResult success;
	
	VkFramebufferCreateInfo framebuffer_info = {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.renderPass = pipeline->render_pass,
		.attachmentCount = 1,
		.pAttachments = image_view,
		.width = extent->width,
		.height = extent->height,
		.layers = 1
	};

	success = vkCreateFramebuffer(device->logical_device,
	                              &framebuffer_info,
	                              NULL,
	                              &framebuffer->handle);

	if (success != VK_SUCCESS)
	{
		LOG_ERROR("vkCreateFramebuffer failed");
		free(framebuffer);
		return NULL;
	}

	return framebuffer;
}

void framebuffer_destroy(Framebuffer *framebuffer, Device *device)
{
	vkDestroyFramebuffer(device->logical_device, framebuffer->handle, NULL);
	free(framebuffer);
}