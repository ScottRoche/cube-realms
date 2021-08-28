#ifndef _DEVICES_H_
#define _DEVICES_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "instance.h"

/******************************************************************************
 * @name  _PhysicalDevice
 * @brief An object to hold physical device. 
******************************************************************************/
struct _PhysicalDevice
{
	VkPhysicalDevice handle;
};
typedef struct _PhysicalDevice PhysicalDevice;

/******************************************************************************
 * @name  _Device
 * @brief A logical represenation of a physical device.
******************************************************************************/
struct _Device
{
	VkDevice handle;
};
typedef struct _Device Device;

/******************************************************************************
 * @name        physical_device_create()
 * @brief       Creates a PhysicalDevice for a suitable device.
 * @param[in]   instance The vulkan instance.
 * @return      A pointer to the PhysicalDevice object.
******************************************************************************/
PhysicalDevice *physical_device_create(Instance *instance);

/******************************************************************************
 * @name        physical_device_destory()
 * @brief       Cleans up and destroys a PhysicalDevice.
 * @param[in]   device The PhysicalDevice to cleanup and destroy.
 * @return      void.
******************************************************************************/
void physical_device_destroy(PhysicalDevice *device);


/******************************************************************************
 * @name        device_create()
 * @brief       Creates a logical device for a PhysicalDevice.
 * @param[in]   physical_device The physical device to represent.
 * @return      A pointer to a Device object. If unsuccessful the value of this
 *              pointer will be NULL.
******************************************************************************/
Device *device_create(PhysicalDevice *physical_device);
/******************************************************************************
 * @name        device_destroy()
 * @brief       Cleans up and destroys a Device.
 * @param[in]   device The Device to cleanup and destroy.
 * @return      void
******************************************************************************/
void device_destroy(Device *device);


#endif /* _DEVICES_H_ */