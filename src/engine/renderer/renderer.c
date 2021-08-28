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
	Device *device;
};

static struct Renderer renderer;

int renderer_init()
{
	renderer.instance = instance_create();
	if (renderer.instance == NULL)
	{
		goto instance_create_fail;
	}

	renderer.device = device_create(renderer.instance);
	if (renderer.device == NULL)
	{
		goto device_create_fail;
	}

	return 1;

device_create_fail:
	instance_destroy(renderer.instance);

instance_create_fail:
	return 0;
}

void renderer_deinit()
{
	device_destroy(renderer.device);
	instance_destroy(renderer.instance);
}