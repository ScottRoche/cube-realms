#include "renderer.h"

#include "instance.h"
#include "core/logger.h"

/******************************************************************************
 * @name Renderer
 * @brief An object to encapsulate properties related to the renderer.
******************************************************************************/
struct Renderer 
{
	Instance *instance;
};

static struct Renderer renderer;

int renderer_init()
{
	renderer.instance = instance_create();
	if (renderer.instance == NULL)
	{
		LOG_ERROR("instance_create failed");
		return 0;
	}

	return 1;
}

void renderer_deinit()
{
	instance_destroy(renderer.instance);
}