#include "swap_chain.h"

#include <stdlib.h>
#include <stdint.h>

#include "core/logger.h"

/******************************************************************************
 * @name       swap_chain_get_surface_format()
 * @brief      Gets a usable surface format for a specified swap chain.
 * @param[in]  swap_chain     The swap chain to query the formats of.
 * @param[out] surface_format The format that was selected, or in case of failure
 *                            this will be set to NULL.
 * @return     An ENGINE_ERROR value will be returned. If a format was successfully
 *             found ENGINE_OK.
******************************************************************************/
static ENGINE_ERROR swap_chain_get_surface_format(const SwapChain *swap_chain,
                                                  VkSurfaceFormatKHR *surface_format)
{
	for(int i = 0; i < swap_chain->support.formats_count; i++)
	{
		VkSurfaceFormatKHR format = swap_chain->support.formats[i];
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB
		    && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			*surface_format = format;
			return ENGINE_OK;
		}
	}
	
	surface_format = NULL;
	return ENGINE_ERROR_INVALID_SWAP_CHAIN_FORMAT;
}

/******************************************************************************
 * @name      swap_chain_get_present_mode()
 * @brief     Gets a present mode for the swap chain to use.
 * @param[in] swap_chain The swapchain to check for a present mode.
 * @return    A present mode to be used for the specified swap chain.
******************************************************************************/
static VkPresentModeKHR swap_chain_get_present_mode(const SwapChain *swap_chain)
{
	for (uint32_t i = 0; i < swap_chain->support.present_modes_count; i++)
	{
		if (swap_chain->support.present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return swap_chain->support.present_modes[i];
		}
	}
	
	LOG_WARNING("No present mode of type VK_PRESENT_MODE_MAILBOX_KHR was found."
	             " Using VK_PRESENT_MODE_FIFO_KHR instead.");
	return VK_PRESENT_MODE_FIFO_KHR;
}

/******************************************************************************
 * @name         clamp_extent_value()
 * @brief        Clamps a value between two values.
 * @param[inout] value The value to clamp.
 * @param        min   The minimum value of the value parameter.
 * @param        max   The maximum value of the value parameter.
 * @return       void
******************************************************************************/
static inline void clamp_extent_value(uint32_t *value, uint32_t min, uint32_t max)
{
	*value = *value >= min ? *value : min;
	*value = *value <= max ? *value : max;
}

/******************************************************************************
 * @name      swap_chain_get_extent()
 * @brief     Get the extent for a given window.
 * @param[in] swap_chain The swap chain to check for an already existing extent.
 * @param[in] window     The window in which to create an extent if one does not
 *                       already exist.
 * @return    A VkExtent2D that already existed or has been created.
******************************************************************************/
static VkExtent2D swap_chain_get_extent(const SwapChain *swap_chain,
                                        const Window *window)
{
	int width, height;
	VkExtent2D actual_extent;

	if (swap_chain->support.capabilities.currentExtent.width != UINT32_MAX)
	{
		return swap_chain->support.capabilities.currentExtent;
	}

	glfwGetFramebufferSize(window->handle, &width, &height);
	actual_extent.width = width;
	actual_extent.height = height;

	clamp_extent_value(&actual_extent.width,
	                   swap_chain->support.capabilities.minImageExtent.width,
	                   swap_chain->support.capabilities.maxImageExtent.width);

	clamp_extent_value(&actual_extent.height,
	                   swap_chain->support.capabilities.minImageExtent.height,
	                   swap_chain->support.capabilities.maxImageExtent.height);

	return actual_extent;
}

/******************************************************************************
 * @name      swap_chain_image_views_create()
 * @brief     Creates image views from swap chain images.
 * @param[in] swap_chain The swap chain to retrieve the images from and the
 *                       image views to be stored in.
 * @param[in] device     The device to create the image views for.
 * @return    An ENGINE_ERROR value. If swap chain image views were created
 *            successfully ENGINE_OK is returned.
******************************************************************************/
static ENGINE_ERROR swap_chain_image_views_create(SwapChain *restrict swap_chain,
                                              const Device *restrict device)
{
	VkImageView *image_views = calloc(swap_chain->image_count, sizeof(VkImageView));

	for (int i = 0; i < swap_chain->image_count; i++)
	{
		VkResult success;

		VkImageViewCreateInfo image_view_info = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = swap_chain->images[i],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = swap_chain->format,
			.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
			.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
			.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.subresourceRange.baseMipLevel = 0,
			.subresourceRange.levelCount = 1,
			.subresourceRange.baseArrayLayer = 0,
			.subresourceRange.layerCount = 1
		};

		success = vkCreateImageView(device->logical_device,
		                            &image_view_info,
		                            NULL,
		                            &image_views[i]);

		if (success != VK_SUCCESS)
		{
			LOG_ERROR("Failed to create an image view for image %d", i);
			for (int j = 0; j <= i; j++)
			{
				vkDestroyImageView(device->logical_device, image_views[j], NULL);
			}
			free(image_views);
			return ENGINE_ERROR_INIT_FAILED;
		}
	}

	swap_chain->image_views = image_views;
	return ENGINE_OK;
}

ENGINE_ERROR swap_chain_create(SwapChain **swap_chain,
                               const Window *restrict window,
                               const Device *restrict device,
                               const VkSurfaceKHR *restrict surface)
{
	*swap_chain = malloc(sizeof(SwapChain));
	VkSurfaceFormatKHR surface_format;
	VkPresentModeKHR present_mode;
	VkExtent2D swap_chain_extent;
	uint32_t image_count = 0;
	uint32_t queue_family_indicies[2];
	ENGINE_ERROR error;
	VkResult success;

	(*swap_chain)->support = device->swap_chain_details;

	error = swap_chain_get_surface_format(*swap_chain, &surface_format);
	if (error != ENGINE_OK)
	{
		free(*swap_chain);
		ENGINE_LOG_RETURN_IF_ERROR(error, "Failed to find valid swap chain surface format");
	}

	present_mode = swap_chain_get_present_mode(*swap_chain);
	swap_chain_extent = swap_chain_get_extent(*swap_chain, window);

	image_count = (*swap_chain)->support.capabilities.minImageCount + 1;
	if ((*swap_chain)->support.capabilities.maxImageCount > 0
	    && image_count > (*swap_chain)->support.capabilities.maxImageCount)
	{
		image_count = (*swap_chain)->support.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR swap_chain_info = {
		.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
		.surface = *surface,
		.minImageCount = image_count,
		.imageFormat = surface_format.format,
		.imageColorSpace = surface_format.colorSpace,
		.imageExtent = swap_chain_extent,
		.imageArrayLayers = 1,
		.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
	};

	queue_family_indicies[0] = (uint32_t)device->queue_family_indicies.graphics_family;
	queue_family_indicies[1] = (uint32_t)device->queue_family_indicies.present_family;

	if (queue_family_indicies[0] != queue_family_indicies[1])
	{
		swap_chain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swap_chain_info.queueFamilyIndexCount = 2;
		swap_chain_info.pQueueFamilyIndices = queue_family_indicies;
	}
	else
	{
		swap_chain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swap_chain_info.queueFamilyIndexCount = 0;
		swap_chain_info.pQueueFamilyIndices = NULL;
	}

	swap_chain_info.preTransform = (*swap_chain)->support.capabilities.currentTransform;
	swap_chain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	swap_chain_info.presentMode = present_mode;
	swap_chain_info.clipped = VK_TRUE;

	swap_chain_info.oldSwapchain = VK_NULL_HANDLE;

	success = vkCreateSwapchainKHR(device->logical_device,
	                               &swap_chain_info,
	                               NULL,
	                               &(*swap_chain)->handle);

	if (success != VK_SUCCESS)
	{
		free(swap_chain);
		if (success & VK_ERROR_OUT_OF_HOST_MEMORY
			|| success == VK_ERROR_OUT_OF_DEVICE_MEMORY)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_OUT_OF_MEMORY,
			                           "Failed to create swap chain, insufficient"
			                           "host/device memory");
		}
		else if (success == VK_ERROR_INITIALIZATION_FAILED)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_INIT_FAILED,
			                           "Failed to initalise instance");
		}
		else if (success == VK_ERROR_DEVICE_LOST)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_DEVICE_LOST,
			                           "Device was lost while creating swap chain");
		}
		else if (success == VK_ERROR_SURFACE_LOST_KHR)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_SURFACE_LOST,
			                           "Surface was lost while creating swap chain");
		}
		else if (success == VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_WINDOW_IN_USE,
			                           "Window is already in use by Vulkan or another API");
		}
	}

	vkGetSwapchainImagesKHR(device->logical_device,
	                        (*swap_chain)->handle,
	                        &(*swap_chain)->image_count,
	                        NULL);

	(*swap_chain)->images = malloc(sizeof(VkImage) * image_count);
	vkGetSwapchainImagesKHR(device->logical_device,
	                        (*swap_chain)->handle,
	                        &(*swap_chain)->image_count,
	                        (*swap_chain)->images);

	(*swap_chain)->format = surface_format.format;
	(*swap_chain)->extent = swap_chain_extent;

	if (swap_chain_image_views_create(*swap_chain, device) != ENGINE_OK)
	{
		vkDestroySwapchainKHR(device->logical_device,
		                      (*swap_chain)->handle,
		                      NULL);
		free((*swap_chain)->images);
		free(*swap_chain);
		return ENGINE_ERROR_INIT_FAILED;
	}

	return ENGINE_OK;
}

void swap_chain_destroy(SwapChain *swap_chain, const Device *device)
{
	for (int i = 0; i < swap_chain->image_count; i++)
	{
		vkDestroyImageView(device->logical_device,
		                   swap_chain->image_views[i],
		                   NULL);
	}

	vkDestroySwapchainKHR(device->logical_device,
	                      swap_chain->handle,
	                      NULL);

	free(swap_chain->image_views);
	free(swap_chain->images);
	free(swap_chain);
}
