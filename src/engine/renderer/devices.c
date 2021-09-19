#include "devices.h"

#include <stdlib.h>
#include <string.h>

#include "core/logger.h"

static const char *device_extensions[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

/******************************************************************************
 * @name        check_device_extension_support()
 * @brief       Checks a device against a set of extensions that are required.
 * @param[in]   device The physical device to check extension support for.
 * @return      An integer to represent the success of the check. If 1 is
 *              returned it was successful, else the physical device does not
 *              have all of the required extensions.
******************************************************************************/
static int check_device_extension_support(VkPhysicalDevice *device)
{
	VkExtensionProperties *extensions;
	uint32_t device_extension_count;
	uint32_t extension_count;
	uint32_t i, j;

	device_extension_count =
		sizeof(device_extensions) / sizeof(device_extensions[0]);

	vkEnumerateDeviceExtensionProperties(*device, NULL, &extension_count, NULL);

	extensions = malloc(sizeof(VkExtensionProperties) * extension_count);
	vkEnumerateDeviceExtensionProperties(*device,
	                                     NULL,
	                                     &extension_count,
	                                     extensions);

	/* Only checks for one extension before returning successful */
	for (i = 0; i < device_extension_count; i++)
	{
		size_t extension_length = strlen(device_extensions[i]);
		for (j = 0; j < extension_count; j++)
		{
			int success = strncmp(device_extensions[i],
			                      extensions[j].extensionName,
			                      extension_length);
			if (success == 0)
			{
				return 1;
			}
		}
	}

	return 0;
}

/******************************************************************************
 * @name      device_query_swap_chain_support_details()
 * @brief     Queries a physical device for swap chain support details including
 *            capabilities, a list of formats and a list of present modes.
 * @param[in] device         The physical device to query.
 * @param[in] render_surface The render surface to query the swap chain details on.
 * @return    A DeviceSwapChainSupportDetails struct with device specific details
 *            about swap chain support.
******************************************************************************/
static struct DeviceSwapChainSupportDetails
	device_query_swap_chain_support_details(VkPhysicalDevice *device,
	                                        const VkSurfaceKHR *render_surface)
{
	struct DeviceSwapChainSupportDetails details = {
		.formats = NULL,
		.formats_count = 0,
		.present_modes = NULL,
		.present_modes_count = 0
	};

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(*device,
	                                          *render_surface,
	                                          &details.capabilities);

	vkGetPhysicalDeviceSurfaceFormatsKHR(*device,
	                                     *render_surface,
	                                     &details.formats_count,
	                                     NULL);

	if (details.formats_count != 0)
	{
		details.formats =
			malloc(sizeof(VkSurfaceFormatKHR) * details.formats_count);
		vkGetPhysicalDeviceSurfaceFormatsKHR(*device,
		                                     *render_surface,
		                                     &details.formats_count,
		                                     details.formats);
	}
	else
	{
		LOG_WARNING("Failed to get device surface formats");
	}

	vkGetPhysicalDeviceSurfacePresentModesKHR(*device,
	                                          *render_surface,
	                                          &details.present_modes_count,
	                                          NULL);

	if (details.present_modes_count != 0)
	{
		details.present_modes =
			malloc(sizeof(VkPresentModeKHR) * details.present_modes_count);
		vkGetPhysicalDeviceSurfacePresentModesKHR(*device,
		                                          *render_surface,
		                                          &details.present_modes_count,
		                                          details.present_modes);
	}
	else
	{
		LOG_WARNING("Failed to get device surface present modes");
	}

	return details;
}

/******************************************************************************
 * @name        find_queue_family()
 * @brief       Finds valid queue families and sets each families index into
 *              indicies.
 * @param[in]   device   The device to find the queue families in.
 * @param[in]   indicies The struct to store the indicies of each queue family.
 * @return      void
******************************************************************************/
static void find_queue_family(VkPhysicalDevice *device,
                              struct QueueFamilyIndicies *indicies,
                              const VkSurfaceKHR *render_surface)
{
	VkQueueFamilyProperties *queue_family_properties;
	VkBool32 present_support;
	uint32_t queue_family_count;
	uint32_t i;

	vkGetPhysicalDeviceQueueFamilyProperties(*device, &queue_family_count, NULL);

	queue_family_properties =
		malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(*device,
	                                         &queue_family_count,
	                                         queue_family_properties);

	for (i = 0; i < queue_family_count; i++)
	{
		vkGetPhysicalDeviceSurfaceSupportKHR(*device,
		                                     i,
		                                     *render_surface,
		                                     &present_support);
		if (present_support)
		{
			indicies->present_family = i;
		}

		if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indicies->graphics_family = i;
		}
	}

	free(queue_family_properties);
}


/******************************************************************************
 * @name        suitable_device_found()
 * @brief       Checks a device for required extensions and queue families.
 * @param[in]   device The device to be check for suitability.
 * @return      A ENGINE_ERROR value. If a suitable device is found ENGINE_OK.
******************************************************************************/
static ENGINE_ERROR suitable_device_found(VkPhysicalDevice *device,
                                          struct QueueFamilyIndicies *queue_family_indicies,
                                          const VkSurfaceKHR *render_surface,
                                          struct DeviceSwapChainSupportDetails *details)
{
	VkPhysicalDeviceProperties device_properties;
	struct QueueFamilyIndicies indicies = {-1, -1};

	vkGetPhysicalDeviceProperties(*device, &device_properties);

	find_queue_family(device, &indicies, render_surface);

	if (!check_device_extension_support(device))
	{
		ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_INVALID_DEVICE,
		                           "Failed to find device with required extensions");
	}

	*details = device_query_swap_chain_support_details(device, render_surface);

	if (indicies.graphics_family != -1 && indicies.present_family != -1
	    && details->formats != NULL && details->present_modes != NULL)
	{
		*queue_family_indicies = indicies;
		return ENGINE_OK;
	}

	return ENGINE_ERROR_INVALID_DEVICE;
}

/******************************************************************************
 * @name      select_physical_device()
 * @brief     Selects the physical device to be used.
 * @param[in] device   The device object in which will store the physical device.
 * @param[in] instance The vulkan instance.
 * @result    An ENGINE_ERROR value. If a physical device is selected ENGINE_OK.
******************************************************************************/
static ENGINE_ERROR select_physical_device(Device *restrict device,
                                           const Instance *restrict instance,
                                           const VkSurfaceKHR *render_surface)
{
	VkPhysicalDevice *devices;
	uint32_t device_count = 0;
	uint32_t i;
	ENGINE_ERROR error;

	vkEnumeratePhysicalDevices(instance->handle, &device_count, NULL);

	if (device_count == 0)
	{
		ENGINE_RETURN_IF_ERROR(ENGINE_ERROR_INVALID_DEVICE);
	}

	devices = malloc(sizeof(VkPhysicalDevice) * device_count);
	vkEnumeratePhysicalDevices(instance->handle,
	                           &device_count,
	                           devices);

	ENGINE_ASSERT(device != NULL);

	for (i = 0; i < device_count; i++)
	{
		struct DeviceSwapChainSupportDetails swap_chain_support;
		error = suitable_device_found(&devices[i],
		                                &device->queue_family_indicies,
		                                render_surface,
		                                &swap_chain_support);
		if (error == ENGINE_OK)
		{
			device->physical_device = devices[i];
			device->swap_chain_details = swap_chain_support;
			free(devices);
			return ENGINE_OK;
		}
	}

	free(devices);
	return ENGINE_ERROR_INVALID_DEVICE;
}

ENGINE_ERROR device_create(Device **restrict device,
                           const Instance *const instance,
                           const VkSurfaceKHR *restrict render_surface)
{
	ENGINE_ERROR error;
	float queue_priority = 1.0f;

	*device = malloc(sizeof(Device));
	(*device)->queue_family_indicies.graphics_family = -1;
	(*device)->queue_family_indicies.present_family = -1;

	error = select_physical_device(*device, instance, render_surface);
	if (error != ENGINE_OK)
	{
		free(*device);
		ENGINE_LOG_RETURN_IF_ERROR(error, "Failed to find valid physical device.");
	}

	// Create a logical device
	VkDeviceQueueCreateInfo graphics_queue_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = (uint32_t)(*device)->queue_family_indicies.graphics_family,
		.queueCount = 1,
		.pQueuePriorities = &queue_priority
	};

	VkDeviceQueueCreateInfo present_queue_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = (uint32_t)(*device)->queue_family_indicies.present_family,
		.queueCount = 1,
		.pQueuePriorities = &queue_priority
	};

	VkPhysicalDeviceFeatures physical_device_features = {};

	VkDeviceQueueCreateInfo queue_create_infos[] = {graphics_queue_info,
	                                                present_queue_info};

	VkDeviceCreateInfo device_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pQueueCreateInfos = queue_create_infos,
		.queueCreateInfoCount = 2,
		.pEnabledFeatures = &physical_device_features,
		.enabledExtensionCount = 1,
		.ppEnabledExtensionNames = device_extensions,
		.enabledLayerCount = 0
	};

	VkResult success = vkCreateDevice((*device)->physical_device,
	                                  &device_info,
	                                  NULL,
	                                  &(*device)->logical_device);
	if (success != VK_SUCCESS)
	{
		free(*device);
		*device = NULL;

		if (success & VK_ERROR_OUT_OF_HOST_MEMORY
			|| success == VK_ERROR_OUT_OF_DEVICE_MEMORY)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_OUT_OF_MEMORY,
			                           "Failed to create device, insufficient"
			                           "host/device memory");
		}
		else if (success == VK_ERROR_FEATURE_NOT_PRESENT
		         || success == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_INIT_FAILED,
			                           "Failed to create instance, feature/extension"
			                           "not found");
		}
		else if (success == VK_ERROR_INITIALIZATION_FAILED)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_INIT_FAILED,
			                           "Failed to initalise device");
		}
		else if (success == VK_ERROR_DEVICE_LOST)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_DEVICE_LOST, "Device was lost!");
		}
	}

	vkGetDeviceQueue((*device)->logical_device,
	                 (*device)->queue_family_indicies.graphics_family,
	                 0,
	                 &(*device)->graphics_queue);

	vkGetDeviceQueue((*device)->logical_device,
	                 (*device)->queue_family_indicies.present_family,
	                 0,
	                 &(*device)->present_queue);

	return ENGINE_OK;
}

void device_destroy(Device *device)
{
	vkDestroyDevice(device->logical_device, NULL);
	free(device->swap_chain_details.formats);
	free(device->swap_chain_details.present_modes);
	free(device);
}