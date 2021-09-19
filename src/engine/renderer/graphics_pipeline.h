#ifndef _GRAPHICS_PIPELINE_H_
#define _GRAPHICS_PIPELINE_H_

#include <stdint.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "core/debug.h"

#include "devices.h"
#include "swap_chain.h"
#include "buffer.h"

struct _GraphicsPipeline
{
	VkPipeline handle;
	VkPipelineShaderStageCreateInfo *shader_stages;
	uint32_t stage_count;
	uint32_t last_stage;

	VkPipelineLayout layout;
	VkRenderPass render_pass;
};
typedef struct _GraphicsPipeline GraphicsPipeline;

/******************************************************************************
 * @name       graphics_pipeline_create()
 * @brief      Creates an instance of the GraphicsPipeline struct.
 * @param[out] pipeline A pointer to a pointer that stores the initalised pipeline.
 * @param[in]  device The device the pipeline will belong to.
 * @param[in]  swap_chain The swap_chain that the pipeline recieves images from.
 * @param[in]  vertex_data Vertex data to be processed.
 * @return     An ENGINE_ERROR value, if creation of the graphics pipeline was 
 *             successful ENGINE_OK is returned.
******************************************************************************/
ENGINE_ERROR graphics_pipeline_create(GraphicsPipeline **pipeline,
                                      const Device *restrict device,
                                      const SwapChain *restrict swap_chain,
                                      const VertexData *restrict vertex_data);

/******************************************************************************
 * @name      graphics_pipeline_destroy()
 * @brief     Destroys a graphics pipeline.
 * @param[in] pipeline The pipeline to be destroyed.
 * @param[in] device   The device the pipeline belongs to.
 * @return    void
******************************************************************************/
void graphics_pipeline_destroy(GraphicsPipeline *restrict pipeline,
                               const Device *restrict device);

/******************************************************************************
 * @name       set_graphics_pipeline_shader()
 * @brief      Creates a shader stage for a given shader binary.
 * @param[in]  pipeline     The pipeline the stage will belong to.
 * @param[in]  device       The device the pipeline belongs to.
 * @param[in]  shader_path  A path to the shader binary.
 * @param      shader_stage The shaders stage flag.
 * @param[out] module       A pointer to a shader module to be cleaned up after
 *                          the graphics pipeline has been created.
 * @return     An ENGINE_ERROR value that describes the success of setting a pipelines
 *             shader. If successful ENGINE_OK.
******************************************************************************/
ENGINE_ERROR set_graphics_pipeline_shader(GraphicsPipeline *restrict pipeline,
                                          const Device *restrict device,
                                          const char *restrict shader_path,
                                          VkShaderStageFlagBits shader_stage,
                                          VkShaderModule *restrict module);

#endif /* _GRAPHICS_PIPELINE_H_ */