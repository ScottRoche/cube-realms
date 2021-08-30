#ifndef _DEVICES_H_
#define _DEVICES_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "instance.h"

/******************************************************************************
 * @name  DeviceSwapChainSupportDetails
 * @brief A struct containing details about a physical devices support for swap
 *        chains.
******************************************************************************/
struct DeviceSwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;

	VkSurfaceFormatKHR *formats;
	uint32_t formats_count;

	VkPresentModeKHR *present_modes;
	uint32_t present_modes_count;
};

/******************************************************************************
 * @name  QueueFamilyIndicies
 * @brief A struct to store indicies of queue families found on a physical device.
******************************************************************************/
struct QueueFamilyIndicies
{
	int64_t graphics_family;
	int64_t present_family;
};

/******************************************************************************
 * @name  _Device
 * @brief A logical represenation of a physical device.
******************************************************************************/
struct _Device
{
	VkDevice logical_device;
	VkPhysicalDevice physical_device;

	struct DeviceSwapChainSupportDetails swap_chain_details;
	struct QueueFamilyIndicies queue_family_indicies;

	VkQueue graphics_queue;
	VkQueue present_queue;
};
typedef struct _Device Device;

/******************************************************************************
 * @name        device_create()
 * @brief       Creates a Device object which holds properties for the physical
 *              and logical device.
 * @param[in]   instance The vulkan instance.
 * @param[in]   render_surface 
 * @return      A pointer to a Device struct.
******************************************************************************/
Device *device_create(const Instance *const instance,
                      const VkSurfaceKHR *render_surface);

/******************************************************************************
 * @name        device_destroy()
 * @brief       Cleans up and destroys a Device.
 * @param[in]   device The Device to cleanup and destroy.
 * @return      void
******************************************************************************/
void device_destroy(Device *device);


#endif /* _DEVICES_H_ */