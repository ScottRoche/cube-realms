#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/debug.h"

/******************************************************************************
 * @name  _Instance
 * @brief Stores a VKInstance.
******************************************************************************/
struct _Instance
{
	VkInstance handle;
};
typedef struct _Instance Instance;

/******************************************************************************
 * @name      instance_create()
 * @brief     Creates an Instance struct that will handle VkInstance operations.
 * @param[out] instance A pointer to a pointer which will store the address of
 *                      of the created instance.
 * @return    A value of ENGINE_ERROR if successful ENGINE_OK.
******************************************************************************/
ENGINE_ERROR instance_create(Instance **instance);

/******************************************************************************
 * @name      instance_destroy()
 * @brief     Destroys an specified Instance
 * @param[in] instance The instance to destroyed.
 * @return    void
******************************************************************************/
void instance_destroy(Instance *instance);

#endif /* _INSTANCE_H_ */