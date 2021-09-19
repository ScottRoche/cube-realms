#include "graphics_pipeline.h"

#include <stdio.h>
#include <stdlib.h>

#include "core/logger.h"

/******************************************************************************
 * @name       read_shader_binary()
 * @brief      Copies the contents of a shader binary and outputs it.
 * @param[in]  file The file path of the binary.
 * @param[out] length The length of the outputted buffer.
 * @return     A pointer to a heap allocated buffer.
******************************************************************************/
static char* read_shader_binary(const char *file, size_t *length)
{
	FILE *fp = fopen(file, "rb");
	char *buffer = NULL;

	if (fp != NULL)
	{
		fseek(fp, 0, SEEK_END);
		*length = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if (length != 0)
		{
			buffer = malloc(sizeof(char) * (*length));
			fread(buffer, sizeof(char), *length, fp);
		}

		fclose(fp);
	}
	else
	{
		LOG_ERROR("Path to binary does not exist");
	}

	return buffer;
}

/******************************************************************************
 * @name      create_render_pass()
 * @brief     Creates a render pass for a pipeline.
 * @param[in] pipeline The pipeline the pass belongs to.
 * @param[in] device The device the pipeline belongs to.
 * @param[in] swap_chain The swap_chain that provides images to the pipeline.
 * @return    An ENGINE_ERROR value that describes the success of creating the
 *            render pass. If successful ENGINE_OK is returned.
******************************************************************************/
static ENGINE_ERROR create_render_pass(GraphicsPipeline *pipeline,
                                       const Device *device,
                                       const SwapChain *swap_chain)
{
	VkResult success;

	VkAttachmentDescription color_attachment = {
		.format = swap_chain->format,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
		.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};

	VkAttachmentReference color_attachment_ref = {
		.attachment = 0,
		.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	};

	VkSubpassDescription subpass = {
		.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
		.colorAttachmentCount = 1,
		.pColorAttachments = &color_attachment_ref
	};

	VkSubpassDependency dependency = {
		.srcSubpass = VK_SUBPASS_EXTERNAL,
		.dstSubpass = 0,
		.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
	};

	VkRenderPassCreateInfo render_pass_info = {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
		.attachmentCount = 1,
		.pAttachments = &color_attachment,
		.subpassCount = 1,
		.pSubpasses = &subpass,
		.dependencyCount = 1,
		.pDependencies = &dependency
	};

	success = vkCreateRenderPass(device->logical_device,
	                             &render_pass_info,
	                             NULL,
	                             &pipeline->render_pass);
	if (success != VK_SUCCESS)
	{
		ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_OUT_OF_MEMORY,
		                           "Failed to create render pass, insufficient"
		                           "host/device memory");
	}

	return ENGINE_OK;
}

ENGINE_ERROR graphics_pipeline_create(GraphicsPipeline **pipeline,
                                      const Device *restrict device,
                                      const SwapChain *restrict swap_chain,
                                      const VertexData *restrict vertex_data)
{
	*pipeline = malloc(sizeof(GraphicsPipeline));
	VkShaderModule vertex_module = NULL;
	VkShaderModule fragment_module = NULL;
	ENGINE_ERROR error;
	VkResult success;

	(*pipeline)->stage_count = 2;
	(*pipeline)->shader_stages =
		calloc((*pipeline)->stage_count, sizeof(VkPipelineShaderStageCreateInfo));
	(*pipeline)->last_stage = 0;

	error = set_graphics_pipeline_shader(*pipeline,
	                                     device,
	                                     "./assets/vert.spv",
	                                     VK_SHADER_STAGE_VERTEX_BIT,
	                                     &vertex_module);

	ENGINE_GOTO_IF_ERROR(error, shader_create_fail);

	error = set_graphics_pipeline_shader(*pipeline,
	                                     device,
	                                     "./assets/frag.spv",
	                                     VK_SHADER_STAGE_FRAGMENT_BIT,
	                                     &fragment_module);

	ENGINE_GOTO_IF_ERROR(error, shader_create_fail);

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertex_data->binding_description,
		.vertexAttributeDescriptionCount = 2,
		.pVertexAttributeDescriptions = vertex_data->attribute_description
	};

	VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};

	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = swap_chain->extent.width,
		.height = swap_chain->extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	VkRect2D scissor = {
		.offset = {0, 0},
		.extent = swap_chain->extent
	};

	VkPipelineViewportStateCreateInfo viewport_state = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.pViewports = &viewport,
		.scissorCount = 1,
		.pScissors = &scissor
	};

	VkPipelineRasterizationStateCreateInfo rastertizer = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.rasterizerDiscardEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.lineWidth = 1.0f, 
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.depthBiasConstantFactor = 0.0f,
		.depthBiasClamp = 0.0f,
		.depthBiasSlopeFactor = 0.0f
	};

	VkPipelineMultisampleStateCreateInfo multisample = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.sampleShadingEnable = VK_FALSE,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.minSampleShading = 1.0f,
		.pSampleMask = NULL,
		.alphaToCoverageEnable = VK_FALSE,
		.alphaToOneEnable = VK_FALSE
	};

	VkPipelineColorBlendAttachmentState color_blend_attachment = {
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT
		                  | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
		.blendEnable = VK_TRUE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD
	};

	VkPipelineColorBlendStateCreateInfo color_blend = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.logicOp = VK_LOGIC_OP_COPY,
		.attachmentCount = 1,
		.pAttachments = &color_blend_attachment,
		.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
	};

	VkPipelineLayoutCreateInfo layout = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
		.setLayoutCount = 0,
		.pSetLayouts = NULL,
		.pushConstantRangeCount = 0,
		.pPushConstantRanges = NULL
	};

	success = vkCreatePipelineLayout(device->logical_device,
	                       &layout,
	                       NULL,
	                       &(*pipeline)->layout);

	if (success != VK_SUCCESS)
	{
		error = ENGINE_ERROR_OUT_OF_MEMORY;
		ENGINE_LOG_GOTO_IF_ERROR(error,
		                         "Failed to create pipeline layout, insufficient"
		                         "host/device memory",
		                         pipeline_layout_create_fail);
	}


	error = create_render_pass(*pipeline, device, swap_chain);
	ENGINE_GOTO_IF_ERROR(error, render_pass_init_fail);

	VkGraphicsPipelineCreateInfo pipeline_info = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.stageCount = 2,
		.pStages = (*pipeline)->shader_stages,
		.pVertexInputState = &vertex_input_info,
		.pInputAssemblyState = &input_assembly_info,
		.pViewportState = &viewport_state,
		.pRasterizationState = &rastertizer,
		.pMultisampleState = &multisample,
		.pDepthStencilState = NULL,
		.pColorBlendState = &color_blend,
		.pDynamicState = NULL,
		.layout = (*pipeline)->layout,
		.renderPass = (*pipeline)->render_pass,
		.subpass = 0,
		.basePipelineHandle = VK_NULL_HANDLE,
		.basePipelineIndex = -1
	};

	success = vkCreateGraphicsPipelines(device->logical_device,
	                                    VK_NULL_HANDLE,
	                                    1,
	                                    &pipeline_info,
	                                    NULL,
	                                    &(*pipeline)->handle);

	if (success != VK_SUCCESS)
	{
		if (success == VK_PIPELINE_COMPILE_REQUIRED_EXT)
		{
			LOG_WARNING("Pipeline compilation suppressed");
		}
		else if (success == VK_ERROR_OUT_OF_HOST_MEMORY
		         || success == VK_ERROR_OUT_OF_DEVICE_MEMORY)
		{
			error = ENGINE_ERROR_OUT_OF_MEMORY;
			ENGINE_LOG_GOTO_IF_ERROR(error,
									"Failed to create pipeline layout, insufficient"
									"host/device memory",
									render_pass_init_fail);
		}
	}

	vkDestroyShaderModule(device->logical_device, vertex_module, NULL);
	vkDestroyShaderModule(device->logical_device, fragment_module, NULL);

	return ENGINE_OK;

render_pass_init_fail:
	vkDestroyPipelineLayout(device->logical_device, (*pipeline)->layout, NULL);

pipeline_layout_create_fail:
	vkDestroyShaderModule(device->logical_device, vertex_module, NULL);
	vkDestroyShaderModule(device->logical_device, fragment_module, NULL);

shader_create_fail:
	free((*pipeline)->shader_stages);
	free(*pipeline);

	return error;
}

void graphics_pipeline_destroy(GraphicsPipeline *restrict pipeline,
                               const Device *restrict device)
{
	vkDestroyPipeline(device->logical_device, pipeline->handle, NULL);
	vkDestroyPipelineLayout(device->logical_device, pipeline->layout, NULL);
	vkDestroyRenderPass(device->logical_device, pipeline->render_pass, NULL);
	free(pipeline->shader_stages);
	free(pipeline);
}

ENGINE_ERROR set_graphics_pipeline_shader(GraphicsPipeline *restrict pipeline,
                                          const Device *restrict device,
                                          const char *restrict shader_path,
                                          VkShaderStageFlagBits shader_stage,
                                          VkShaderModule *restrict module)
{
	size_t shader_length = 0;
	uint32_t *shader_binary = (uint32_t*)read_shader_binary(shader_path,
	                                                        &shader_length);
	VkResult success;

	if (shader_binary == NULL)
	{
		free(shader_binary);
		return ENGINE_ERROR_INIT_FAILED;
	}

	VkShaderModuleCreateInfo module_info = {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.codeSize = shader_length,
		.pCode = shader_binary
	};

	success = vkCreateShaderModule(device->logical_device,
	                               &module_info,
	                               NULL,
	                               module);
	if (success != VK_SUCCESS)
	{
		free(shader_binary);

		if (success == VK_ERROR_OUT_OF_HOST_MEMORY
		    || success == VK_ERROR_OUT_OF_DEVICE_MEMORY)
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_OUT_OF_MEMORY,
			                           "Failed to create shader module, insufficent "
			                           "host/device memory");
		}
		else
		{
			ENGINE_LOG_RETURN_IF_ERROR(ENGINE_ERROR_INIT_FAILED,
			                           "One or more shaders failed to compile or link");
		}
	}

	free(shader_binary);

	VkPipelineShaderStageCreateInfo stage_info = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = shader_stage,
		.module = *module,
		.pName = "main"
	};

	for (int i = 0; i < pipeline->stage_count; i++)
	{
		if (pipeline->shader_stages[i].stage == shader_stage
		    || pipeline->last_stage == i)
		{
			if (pipeline->last_stage == i)
			{
				pipeline->last_stage++;
			}

			pipeline->shader_stages[i] = stage_info;
			return ENGINE_OK;
		}
	}

	LOG_ERROR("Failed to add the shader stage to the pipeline's shader stages");
	return ENGINE_ERROR_INIT_FAILED;
}