#include "renderer.h"

#include "core/logger.h"

#include "instance.h"
#include "devices.h"

/******************************************************************************
 * @name Renderer
 * @brief An object to encapsulate properties related to the renderer.
******************************************************************************/
struct Renderer 
{
	Instance *instance;
	PhysicalDevice *physical_device;
};

static struct Renderer renderer;

int renderer_init()
{
	renderer.instance = instance_create();
	if (renderer.instance == NULL)
	{
		LOG_ERROR("instance_create failed");
		goto instance_create_fail;
	}

	renderer.physical_device = physical_device_create(renderer.instance);
	if (renderer.physical_device == NULL)
	{
		LOG_ERROR("physical_device_create failed");
		goto physical_device_create_fail;
	}

	return 1;

physical_device_create_fail:
	instance_destroy(renderer.instance);
instance_create_fail:
	return 0;
}

void renderer_deinit()
{
	instance_destroy(renderer.instance);
	physical_device_destroy(renderer.physical_device);
}