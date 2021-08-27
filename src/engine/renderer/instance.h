#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

/******************************************************************************
 * @name  _Instance
 * @brief An object to hold properties associated with an application.
******************************************************************************/
struct _Instance
{
	VkInstance handle;
};
typedef struct _Instance Instance;

/******************************************************************************
 * @name   instance_create()
 * @brief  Creates an Instance object which encapsulates a VkInstance.
 * @return A pointer to the heap allocate Instance.
******************************************************************************/
Instance *instance_create();

/******************************************************************************
 * @name   instance_destroy()
 * @brief  Destroys an Instance object deinitalising it, and freeing memory 
 *         associated with it.
 * @return void
******************************************************************************/
void instance_destroy(Instance *instance);

#endif /* _INSTANCE_H_ */