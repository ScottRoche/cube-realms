#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "devices.h"
#include "graphics_pipeline.h"

struct _Framebuffer
{
	VkFramebuffer handle;
};
typedef struct _Framebuffer Framebuffer;

Framebuffer *framebuffer_create(const Device *restrict device,
                                const GraphicsPipeline *restrict pipeline,
                                const VkImageView *restrict image_view,
                                const VkExtent2D *restrict extent);

void framebuffer_destroy(Framebuffer *restrict framebuffer,
                         Device *restrict device);

#endif /* _FRAMEBUFFER_H_ */