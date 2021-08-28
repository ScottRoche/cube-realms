#include "devices.h"

#include <stdlib.h>
#include <string.h>

#include "core/logger.h"

/******************************************************************************
 * @name  QueueFamilyIndicies
 * @brief A struct to store indicies of queue families found on a physical device.
 * 
 * @TODO: Abstract queues/queue families into its own file as these will be used
 * all around the project and are not confined to this file.
******************************************************************************/
struct QueueFamilyIndicies
{
	int graphics_family;
	int present_family;
};

/******************************************************************************
 * Physical Devices
******************************************************************************/
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
                              struct QueueFamilyIndicies *indicies)
{
	VkQueueFamilyProperties *queue_family_properties;
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
		/**
		 * TODO: call vkGetPhysicalDeviceSurfaceSupportKHR and deal with
		 * present family support.
		 */
		if (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indicies->graphics_family = i;
		}
	}

	free(queue_family_properties);
}

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
	static const char *device_extensions[] = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

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
 * @name        suitable_device_found()
 * @brief       Checks a device for required extensions and queue families.
 * @param[in]   device The device to be check for suitability.
 * @return      An integer to represent the success of the check. If 1 is returned
 *              it was successful, else the device wasn't suitable.
******************************************************************************/
static int suitable_device_found(VkPhysicalDevice *device)
{
	VkPhysicalDeviceProperties device_properties;
	struct QueueFamilyIndicies indicies = {-1, -1};

	vkGetPhysicalDeviceProperties(*device, &device_properties);

	find_queue_family(device, &indicies);

	if (!check_device_extension_support(device))
	{
		LOG_ERROR("Failed to find required device extensions");
		return 0;
	}

	if (indicies.graphics_family != -1 && indicies.present_family != -1)
	{
		return 1;
	}

	return 0;
}

PhysicalDevice *physical_device_create(Instance *instance)
{
	PhysicalDevice *device = malloc(sizeof(PhysicalDevice));
	VkPhysicalDevice *possible_devices;
	uint32_t device_count;
	uint32_t i;

	vkEnumeratePhysicalDevices(instance->handle, &device_count, NULL);

	if (device_count == 0)
	{
		LOG_ERROR("Failed to find device with Vulkan support")
		free(device);
		return NULL;
	}

	possible_devices = malloc(sizeof(VkPhysicalDevice) * device_count);
	vkEnumeratePhysicalDevices(instance->handle, &device_count, possible_devices);

	for (i = 0; i < device_count; i++)
	{
		if (suitable_device_found(&possible_devices[i]))
		{
			device->handle = possible_devices[i];
			break;
		}
	}

	if (device->handle == VK_NULL_HANDLE)
	{
		LOG_ERROR("Failed to find suitable device\n");
		free(device);
		return NULL;
	}

	return device;
}

void physical_device_destroy(PhysicalDevice *device)
{
	free(device);
}

/******************************************************************************
 * Logic Devices
******************************************************************************/
Device *device_create(PhysicalDevice *physical_device)
{
	Device *device = malloc(sizeof(Device));

	return device;
}

void device_destroy(Device *device)
{
	vkDestroyDevice(device->handle, NULL);
	free(device);
}