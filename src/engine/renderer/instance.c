#include "instance.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "core/logger.h"

static const char *const validation_layers[] = {
	"VK_LAYER_KHRONOS_validation"
};

/******************************************************************************
 * @name       get_required_extensions()
 * @brief      Used to get the required extensions to be used by the vulkan instance.
 * @param[out] count    The number of required extensions.
 * @return     A pointer array of strings containing the extensions.
******************************************************************************/
static const char **get_required_extensions(uint32_t *count)
{
	const char **extensions = glfwGetRequiredInstanceExtensions(count);
	uint32_t i;

	if (extensions == NULL)
	{
		return NULL;
	}

#if defined (DEBUG)
	const char *debug_header = "Required Extensions:\n";
	char extension_list[*count * 64];

	memset(extension_list, 0, sizeof(extension_list));
	for (i = 0; i < *count; i++)
	{
		char temp_string[64];
		
		if (i < *count - 1)
		{
			snprintf(temp_string, (size_t)64, "\t\t%s\n", extensions[i]);
		}
		else
		{
			snprintf(temp_string, (size_t)64, "\t\t%s", extensions[i]);
		}
		strncat(extension_list, temp_string, (size_t)64);
	}

	LOG_DEBUG("%s%s", debug_header, extension_list);
#endif

	return extensions;
}

/******************************************************************************
 * @name    get_required_extensions()
 * @brief   Used to check that the validation layers are valid.
 * @return  An integer value, if this value is 0 no validation layers were
 *          valid, if 1 a validation layer was found.
******************************************************************************/
static int validate_validation_layers()
{
	uint32_t count, layer_found = 0;
	VkLayerProperties *available_layers;
	size_t i, j;
	size_t validation_layer_count =
		sizeof(validation_layers) / sizeof(validation_layers[0]);

	vkEnumerateInstanceLayerProperties(&count, NULL);

	available_layers = malloc(sizeof(VkLayerProperties) * count);
	vkEnumerateInstanceLayerProperties(&count, available_layers);

	for (i = 0; i < validation_layer_count; i++)
	{
		for (j = 0; j < count; j++)
		{
			if (strcmp(validation_layers[i], available_layers[j].layerName) == 0)
			{
				layer_found = 1;
				break;
			}
		}
	}

	if (layer_found == 0)
	{
		LOG_WARNING("Couldn't find any valid validation layers");
	}

	free(available_layers);
	return layer_found;
}

Instance *instance_create()
{
	Instance *instance;
	const char **extensions;
	uint32_t extension_count;
	VkResult success;

	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Cube Realm",
		.applicationVersion = VK_MAKE_VERSION(0, 0, 1),
		.pEngineName = "Cube Realm Engine",
		.engineVersion = VK_MAKE_VERSION(0, 0, 1),
		.apiVersion = VK_API_VERSION_1_0
	};

	extensions = get_required_extensions(&extension_count);
	if (extensions == NULL)
	{
		LOG_ERROR("get_required_extensions() failed to retrieve extensions");
		return NULL;
	}

	validate_validation_layers();

	VkInstanceCreateInfo instance_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app_info,
		.enabledExtensionCount = extension_count,
		.ppEnabledExtensionNames = extensions,
	};

#if defined (DEBUG)
	if (validate_validation_layers())
	{
		instance_info.ppEnabledLayerNames = validation_layers;
		instance_info.enabledLayerCount = 1;
	}
#endif

	instance = malloc(sizeof(Instance));
	success = vkCreateInstance(&instance_info, NULL, &instance->handle);
	if (success != VK_SUCCESS)
	{
		LOG_ERROR("vkCreateInstance failed: %u", success);
		free(instance);
		return NULL;
	}

	return instance;
}

void instance_destroy(Instance *instance)
{
	vkDestroyInstance(instance->handle, NULL);
	free(instance);
}