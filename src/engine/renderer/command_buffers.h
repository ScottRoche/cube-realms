#ifndef _COMMAND_BUFFER_H_
#define _COMMAND_BUFFER_H_

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "devices.h"
#include "graphics_pipeline.h"
#include "swap_chain.h"
#include "framebuffer.h"

/******************************************************************************
 * @name _CommandPool
 * @brief A struct to hold a VkCommandPool.
******************************************************************************/
struct _CommandPool 
{
	VkCommandPool handle;
};
typedef struct _CommandPool CommandPool;

/******************************************************************************
 * @name  _CommandBuffer
 * @brief A struct where an array of VkCommandBuffers is stored.
******************************************************************************/
struct _CommandBuffer
{
	VkCommandBuffer *buffers;
	uint32_t buffer_count;
};
typedef struct _CommandBuffer CommandBuffer;

/******************************************************************************
 * @name      command_pool_create()
 * @brief     Creates a command pool.
 * @param[in] device The device the command pool belongs to.
 * @return    A pointer to a command pool.
******************************************************************************/
CommandPool *command_pool_create(Device *device);

/******************************************************************************
 * @name      command_pool_destroy()
 * @brief     Destroys a specified command pool.
 * @param[in] pool The pool to be destroyed.
 * @param[in] device The device the pool belonged to.
 * @return    void
******************************************************************************/
void command_pool_destroy(CommandPool *pool, Device *device);

/******************************************************************************
 * @name      command_buffer_create()
 * @brief     Creates a set of command buffers for a set of image views for a
 *            given swap chain.
 * @param[in] pool              The pool from which to allocate command buffers.
 * @param[in] device            The device the pool belongs to.
 * @param[in] pipeline          The graphics pipeline to use.
 * @param[in] swap_chain        The swap chain to create command buffers for.
 * @param[in] framebuffer_array An array of framebuffer pointers.
 * @return    A pointer to an array of command buffers.
******************************************************************************/
CommandBuffer *command_buffer_create(const CommandPool *restrict pool,
                                     const Device *restrict device,
                                     const GraphicsPipeline *restrict pipeline,
                                     SwapChain *restrict swap_chain,
                                     Framebuffer **framebuffer_array);

/******************************************************************************
 * @name      command_buffer_destroy()
 * @brief     Destroys a specified command buffer.
 * @param[in] buffer The command buffer to destroy.
 * @param[in] pool   The pool from which the command buffer was allocated.
 * @param[in] device The device the pool belongs to.
 * @return    void
******************************************************************************/
void command_buffer_destroy(CommandBuffer *buffer,
                            CommandPool *pool,
                            Device *device);

#endif /* _COMMAND_BUFFER_H_ */