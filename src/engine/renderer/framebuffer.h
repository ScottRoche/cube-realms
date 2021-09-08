#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "devices.h"
#include "graphics_pipeline.h"

/******************************************************************************
 * @name  _Framebuffer
 * @brief A struct to hold a VkFramebuffer.
******************************************************************************/
struct _Framebuffer
{
	VkFramebuffer handle;
};
typedef struct _Framebuffer Framebuffer;

/******************************************************************************
 * @name      framebuffer_create()
 * @brief     Creates a framebuffer.
 * @param[in] device     The device that the framebuffer belongs to.
 * @param[in] pipeline   The pipeline that will be used with the framebuffer.
 * @param[in] image_view The image view that the framebuffer will display.
 * @param[in] extent     The extent of the framebuffer.
 * @return    A pointer to a framebuffer.
******************************************************************************/
Framebuffer *framebuffer_create(const Device *restrict device,
                                const GraphicsPipeline *restrict pipeline,
                                const VkImageView *restrict image_view,
                                const VkExtent2D *restrict extent);

/******************************************************************************
 * @name      framebuffer_destroy()
 * @brief     Destroys a specified framebuffer.
 * @param[in] framebuffer The framebuffer to destroy.
 * @param[in] device      The device for which the framebuffer belongs to.
 * @return    void
******************************************************************************/
void framebuffer_destroy(Framebuffer *restrict framebuffer,
                         Device *restrict device);

#endif /* _FRAMEBUFFER_H_ */