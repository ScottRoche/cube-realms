#ifndef _SWAP_CHAIN_H_
#define _SWAP_CHAIN_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/window.h"
#include "devices.h"

/******************************************************************************
 * @name _SwapChain
 * @brief A struct to store properties of swap chain.
******************************************************************************/
struct _SwapChain
{
	VkSwapchainKHR handle;
	struct DeviceSwapChainSupportDetails support;

	VkImage *images;
	VkImageView *image_views;
	uint32_t image_count;

	VkFormat format;
	VkExtent2D extent;
};
typedef struct _SwapChain SwapChain;

/******************************************************************************
 * @name      swap_chain_create()
 * @brief     Creates a swap chain.
 * @param[in] window The window that the swap chain images will be created for.
 * @param[in] device The device that the swap chain belongs to.
 * @param[in] surface The surface that images will be presented to.
 * @return    A pointer to a SwapChain object.
******************************************************************************/
SwapChain *swap_chain_create(const Window *restrict window,
                             const Device *restrict device,
                             const VkSurfaceKHR *restrict surface);

/******************************************************************************
 * @name      swap_chain_destroy()
 * @brief     Destroys a specified swap chain.
 * @param[in] swap_chain The swapchain to destroy
 * @param[in] device     The device that the swap chain belongs to.
 * @return    void
******************************************************************************/
void swap_chain_destroy(SwapChain *swap_chain, const Device *device);

#endif /* _SWAP_CHAIN_H_ */