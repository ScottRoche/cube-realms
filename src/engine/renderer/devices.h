#ifndef _DEVICES_H_
#define _DEVICES_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "instance.h"

/******************************************************************************
 * @name  QueueFamilyIndicies
 * @brief A struct to store indicies of queue families found on a physical device.
 * 
 * @TODO: Abstract queues/queue families into its own file as these will be used
 * all around the project and are not confined to this file.
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

	struct QueueFamilyIndicies queue_family_indicies;

	VkQueue graphics_queue;
};
typedef struct _Device Device;

/******************************************************************************
 * @name        device_create()
 * @brief       Creates a logical device for a PhysicalDevice.
 * @param[in]   physical_device The physical device to represent.
 * @return      A pointer to a Device object. If unsuccessful the value of this
 *              pointer will be NULL.
******************************************************************************/
Device *device_create(const Instance *const instance);

/******************************************************************************
 * @name        device_destroy()
 * @brief       Cleans up and destroys a Device.
 * @param[in]   device The Device to cleanup and destroy.
 * @return      void
******************************************************************************/
void device_destroy(Device *device);


#endif /* _DEVICES_H_ */