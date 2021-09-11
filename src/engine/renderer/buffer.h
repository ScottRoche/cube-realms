#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <stdint.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "devices.h"

/* TODO: Have a types header. */
typedef struct _Vector { float x, y; } Vector;
typedef struct _Vector3 { float x, y, z; } Vector3;

/******************************************************************************
 * @name  _VertexData
 * @brief A struct to organise vertex data.
******************************************************************************/
struct _VertexData
{
	Vector *positions;
	Vector3 *colours;

	uint32_t vertex_count;
	
	VkVertexInputBindingDescription binding_description;
	VkVertexInputAttributeDescription attribute_description[2];
};
typedef struct _VertexData VertexData;

/******************************************************************************
 * @name  _VertexBuffer
 * @brief A buffer allocated on a device (GPU) to store vertex data
******************************************************************************/
struct _VertexBuffer
{
	VkBuffer handle;
	VkDeviceMemory buffer_memory;
};
typedef struct _VertexBuffer VertexBuffer;

/******************************************************************************
 * @name      vertex_data_create()
 * @brief     Creates a VertexData struct that stores vertex positions and 
 *            colour data. It also initialises a binding and two attribute
 *            descriptions.
 * @param[in] verticies    The raw vertex data as a float array.
 * @param     vertex_count The number of verticies held within verticies.
 * @return    A pointer to a VertexData object.
******************************************************************************/
VertexData *vertex_data_create(float *verticies, uint32_t vertex_count);

/******************************************************************************
 * @name      vertex_data_destroy()
 * @brief     Destroys a specified VertexData struct.
 * @param[in] data A pointer to a VertexData struct to destroy.
 * @return    void
******************************************************************************/
void vertex_data_destroy(VertexData *data);

/******************************************************************************
 * @name      vertex_buffer_create()
 * @brief     Creates a VertexBuffer and allocates memory for it.
 * @param[in] device        The device the buffer is allocated on.
 * @param     vertices_size The size of the buffer to be allocated.
 * @return    A pointer to a vertex buffer.
******************************************************************************/
VertexBuffer *vertex_buffer_create(Device *device, size_t verticies_size);

/******************************************************************************
 * @name      vertex_buffer_destroy()
 * @param[in] buffer The buffer to be destroyed.
 * @param[in] device The device where to buffer's memory was allocated.
 * @return    void
******************************************************************************/
void vertex_buffer_destroy(VertexBuffer *buffer, Device *device);

#endif /* _BUFFER_H_ */