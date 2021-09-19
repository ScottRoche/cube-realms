#ifndef _DEVICES_H_
#define _DEVICES_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/debug.h"

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
 * @brief A struct to represents a graphics device.
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
 * @name       device_create()
 * @brief      Creates a Device object which holds properties for the physical
 *             and logical device.
 * @param[out] device         A pointer to a pointer which is set to store the address of
 *                            the created device struct.
 * @param[in]  instance       The vulkan instance.
 * @param[in]  render_surface The surface that will be used to draw to.
 * @return     An ENGINE_ERROR value to describe the success of the device struct
 *             creation and initalisation.
******************************************************************************/
ENGINE_ERROR device_create(Device **restrict device,
                           const Instance *const instance,
                           const VkSurfaceKHR *restrict render_surface);

/******************************************************************************
 * @name        device_destroy()
 * @brief       Cleans up and destroys a Device.
 * @param[in]   device The Device to cleanup and destroy.
 * @return      void
******************************************************************************/
void device_destroy(Device *device);


#endif /* _DEVICES_H_ */