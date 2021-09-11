#include "buffer.h"

#include <stdio.h>
#include <stdlib.h>

#include "core/logger.h"

VertexData *vertex_data_create(float *verticies, uint32_t vertex_count)
{
	VertexData *vertex_data = malloc(sizeof(VertexData));
	vertex_data->positions = calloc(vertex_count, sizeof(Vector));
	vertex_data->colours = calloc(vertex_count, sizeof(Vector3));

	for (int i = 0; i < 5 * vertex_count; i += 5)
	{
		uint32_t vertex_id = i / 5;
		Vector position = {verticies[i], verticies[i + 1]};
		Vector3 colour = {verticies[i + 2], verticies[i + 3], verticies[i + 4]};

		vertex_data->positions[vertex_id] = position;
		vertex_data->colours[vertex_id] = colour;
	}

	vertex_data->vertex_count = vertex_count;

	VkVertexInputBindingDescription binding_description = {
		.binding = 0, /* Only allows one binding across all vertex buffers (really not good).*/
		.stride = (sizeof(Vector) + sizeof(Vector3)),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	vertex_data->binding_description = binding_description;

	VkVertexInputAttributeDescription positions_attribute_description = {
		.binding = 0,
		.location = 0,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = offsetof(VertexData, positions)
	};

	VkVertexInputAttributeDescription colour_attribute_description = {
		.binding = 0,
		.location = 1,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(VertexData, colours)
	};

	vertex_data->attribute_description[0] = positions_attribute_description;
	vertex_data->attribute_description[1] = colour_attribute_description;

	return vertex_data;
}

void vertex_data_destroy(VertexData *data)
{
	free(data->positions);
	free(data->colours);
	free(data);
}

static uint8_t find_memory_type(Device *device,
                                 uint32_t filter_type,
                                 VkMemoryPropertyFlags properties,
                                 uint32_t *suitable_type_index)
{
	VkPhysicalDeviceMemoryProperties memory_properties;
	vkGetPhysicalDeviceMemoryProperties(device->physical_device, &memory_properties);

	for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
	{
		if ((filter_type & (1 << i))
		    && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			*suitable_type_index = i;
			return 1;
		}
	}

	LOG_ERROR("Failed to find valid memory filter");
	return 0;
}

VertexBuffer *vertex_buffer_create(Device *device, size_t verticies_size)
{
	VertexBuffer *buffer = malloc(sizeof(VertexBuffer));
	uint32_t suitable_type_index;
	VkResult success;

	VkBufferCreateInfo buffer_info = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = verticies_size,
		.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	success = vkCreateBuffer(device->logical_device,
	                         &buffer_info,
	                         NULL,
	                         &buffer->handle);

	if (success != VK_SUCCESS)
	{
		LOG_ERROR("Failed to create vertex buffer")
		free(buffer);
		return NULL;
	}

	VkMemoryRequirements memory_requirements;
	vkGetBufferMemoryRequirements(device->logical_device,
	                              buffer->handle,
	                              &memory_requirements);

	uint8_t found = find_memory_type(device,
	                                 memory_requirements.memoryTypeBits,
	                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
	                                 | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
	                                 &suitable_type_index);

	if (!found)
	{
		vkDestroyBuffer(device->logical_device, buffer->handle, NULL);
		free(buffer);
		return NULL;
	}

	VkMemoryAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memory_requirements.size,
		.memoryTypeIndex = suitable_type_index,
	};

	success = vkAllocateMemory(device->logical_device,
	                           &alloc_info,
	                           NULL,
	                           &buffer->buffer_memory);

	if (success != VK_SUCCESS)
	{
		LOG_ERROR("Failed to allocate memory for vertex buffer");
		vkDestroyBuffer(device->logical_device, buffer->handle, NULL);
		free(buffer);
		return NULL;
	}

	success = vkBindBufferMemory(device->logical_device,
	                             buffer->handle,
	                             buffer->buffer_memory,
	                             0);

	if (success != VK_SUCCESS)
	{
		LOG_WARNING("Unable to bind vertex buffer to memory");
	}

	return buffer;
}

void vertex_buffer_destroy(VertexBuffer *buffer, Device *device)
{
	vkDestroyBuffer(device->logical_device, buffer->handle, NULL);
	vkFreeMemory(device->logical_device, buffer->buffer_memory, NULL);
	free(buffer);
}