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
 * @name        find_queue_family()
 * @brief       Finds valid queue families and sets each families index into
 *              indicies.
 * @param[in]   device   The device to find the queue families in.
 * @param[in]   indicies The struct to store the indicies of each queue family.
 * @return      void
 * 
 * TODO: Should be abstracted out of this file to be with theQueueFamilyIndicies
 * struct when that happens.
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
 * @return      An integer to represent the success of the check. If 1 is returned
 *              it was successful, else the device wasn't suitable.
******************************************************************************/
static int suitable_device_found(VkPhysicalDevice *device,
                                 struct QueueFamilyIndicies *queue_family_indicies,
                                 const VkSurfaceKHR *render_surface)
{
	VkPhysicalDeviceProperties device_properties;
	struct QueueFamilyIndicies indicies = {-1, -1};

	vkGetPhysicalDeviceProperties(*device, &device_properties);

	find_queue_family(device, &indicies, render_surface);

	if (!check_device_extension_support(device))
	{
		LOG_ERROR("Failed to find required device extensions");
		return 0;
	}

	/* TODO: check for present family. */
	if (indicies.graphics_family != -1 && indicies.present_family != -1)
	{
		*queue_family_indicies = indicies;
		return 1;
	}

	return 0;
}

/******************************************************************************
 * @name      select_physical_device()
 * @brief     Selects the physical device to be used.
 * @param[in] device   The device object in which will store the physical device.
 * @param[in] instance The vulkan instance.
 * @result    An integer value representitive of the success. 1 if successful,
 *            0 if the selection failed.
******************************************************************************/
static int select_physical_device(Device *restrict device,
                                  const Instance *restrict instance,
                                  const VkSurfaceKHR *render_surface)
{
	VkPhysicalDevice *devices;
	uint32_t device_count = 0;
	uint32_t i;
	int success;

	vkEnumeratePhysicalDevices(instance->handle, &device_count, NULL);

	if (device_count == 0)
	{
		LOG_ERROR("Failed to find any devices with Vulkan support")
		free(device);
		return 0;
	}

	devices = malloc(sizeof(VkPhysicalDevice) * device_count);
	vkEnumeratePhysicalDevices(instance->handle,
	                           &device_count,
	                           devices);

	for (i = 0; i < device_count; i++)
	{
		success = suitable_device_found(&devices[i],
		                                &device->queue_family_indicies,
		                                render_surface);
		if (success)
		{
			device->physical_device = devices[i];
			free(devices);
			return 1;
		}
	}

	free(devices);
	return 0;
}

Device *device_create(const Instance *const instance,
                      const VkSurfaceKHR *render_surface)
{
	Device *device = malloc(sizeof(Device));
	uint32_t success;
	float queue_priority = 1.0f;

	device->queue_family_indicies.graphics_family = -1;
	device->queue_family_indicies.present_family = -1;

	success = select_physical_device(device, instance, render_surface);
	if (success == 0)
	{
		LOG_ERROR("Failed to select a physical device")
		free(device);
		return NULL;
	}

	// Create a logical device
	VkDeviceQueueCreateInfo graphics_queue_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = (uint32_t)device->queue_family_indicies.graphics_family,
		.queueCount = 1,
		.pQueuePriorities = &queue_priority
	};

	VkDeviceQueueCreateInfo present_queue_info = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = (uint32_t)device->queue_family_indicies.present_family,
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

	success = vkCreateDevice(device->physical_device,
	                         &device_info,
	                         NULL,
	                         &device->logical_device);
	if (success != VK_SUCCESS)
	{
		LOG_ERROR("vkCreateDevice failed")
		free(device);
		return NULL;
	}

	vkGetDeviceQueue(device->logical_device,
	                 device->queue_family_indicies.graphics_family,
	                 0,
	                 &device->graphics_queue);

	vkGetDeviceQueue(device->logical_device,
	                 device->queue_family_indicies.present_family,
	                 0,
	                 &device->present_queue);

	return device;
}

void device_destroy(Device *device)
{
	vkDestroyDevice(device->logical_device, NULL);
	free(device);
}