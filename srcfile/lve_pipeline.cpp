#include "lve_pipeline.h"

#include <fstream>
#include <stdexcept>
#include <iostream> 
#include <cassert>

namespace lve {

	LvePipeline::LvePipeline(
		LveDevice& device,
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo)
		: lveDevice(device)//成员初始值设定列表初始化设备
	{
		createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
	}//构造函数，传递参数

	LvePipeline::~LvePipeline() {
		vkDestroyShaderModule(lveDevice.device(), vertShaderModule, nullptr);
		vkDestroyShaderModule(lveDevice.device(), fragShaderModule, nullptr);
		vkDestroyPipeline(lveDevice.device(), graphicsPipeline, nullptr);
	}//清理vulkan对象

	std::vector<char> LvePipeline::readFile(const std::string& filepath) {
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);//读取着色器文件

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file: " + filepath);
		}//检查文件是否打开

		size_t fileSize = static_cast<size_t>(file.tellg());//获取文件大小
		std::vector<char> buffer(fileSize);//字符缓冲区

		file.seekg(0);
		file.read(buffer.data(), fileSize);//寻找文件开头并读取

		file.close();//关闭文件
		return buffer;
	}

	void LvePipeline::createGraphicsPipeline(const std::string& vertFilepath,
		const std::string& fragFilepath, 
		const PipelineConfigInfo& configInfo) {

		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Can not create graphics pipeline: no pipelineLayout provided in connfigInfo.");
		assert(configInfo.renderPass != VK_NULL_HANDLE && "Can not create graphics pipeline: no renderPass provided in connfigInfo.");
		auto vertCode = readFile(vertFilepath);
		auto fragCode = readFile(fragFilepath);//读取顶点以及片段代码
		//创建图形管道函数以及类名

		createShaderModule(vertCode, &vertShaderModule);
		createShaderModule(fragCode, &fragShaderModule);//初始化着色器模块

		VkPipelineShaderStageCreateInfo shaderStages[2];//为着色器设置更多创建信息
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;//第一个实例类型设置
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;//着色器阶段顶点位
		shaderStages[0].module = vertShaderModule;//变量设置为等于vert着色器模块
		shaderStages[0].pName = "main";//顶点着色器入口函数名称
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;//frag着色器
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};//解释顶点缓冲区数据，初始输入到图形管道
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;//设置为vk结构类型管道顶点输入状态
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;//顶点与顶点绑定描述计数设置为0
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &configInfo.viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &configInfo.scissor;
		//将视口和剪刀组合成单个视口状态

		VkGraphicsPipelineCreateInfo pipelineInfo{};//配置所有图形管道对象
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;//成员变量，指定使用多少可编程阶段
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;//管道创建信息连接到配置信息
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = nullptr;

		pipelineInfo.layout = configInfo.pipelineLayout;//布局渲染通道，子通道索引传递
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineIndex = -1;//基本管道句柄
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;//基本管道索引

		if (vkCreateGraphicsPipelines(lveDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline"); 
		}

;	}

	void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;//着色器创建信息
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());//指向代码的指针，需要对数据使用重新解释转换，Vulkan需要uint32类型

		if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module");
		}
	}//本地vulkan着色器模块
	//vulkan不使用参数调用函数，而是配置一个结构体并使用指向它的指针调用一个函数

	void LvePipeline::bind(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);//绑定管道
	}

	PipelineConfigInfo LvePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
		PipelineConfigInfo configInfo{};

		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;//s类型设置为vk结构类型
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//拓扑成员为vk基元拓扑三角形列表
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;//基元重新启动启用值为VK_FALSE

		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);//gl位置转换为输出图像
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;
		//配置视口

		configInfo.scissor.offset = { 0, 0 };
		configInfo.scissor.extent = { width, height };
		//配置剪刀


		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;//设置结构类型成员
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;//深度钳位启动设置，强制gl位置z分量在0-1，小于0，钳位到0，大于1，钳位到1
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;//光栅化之前丢弃所有基元
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;//多边形模型
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;//使用以上两个属性唯一标识看到的三角形的那一侧，需要指定逆时针或顺时针
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;
		//光栅化阶段,将几何体分解为每个像素或三角形重叠的片段

		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;
		configInfo.multisampleInfo.pSampleMask = nullptr;
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;
		//多重采样

		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;//启用颜色混合，可以设置如何混合颜色的值，将当前输出与缓冲区中已有颜色值进行混合
		configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
		configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
		configInfo.colorBlendInfo.attachmentCount = 1;
		configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
		configInfo.colorBlendInfo.blendConstants[0] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[1] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[2] = 0.0f;
		configInfo.colorBlendInfo.blendConstants[3] = 0.0f;
 		//颜色混合控制,在帧缓冲区中组合颜色

		configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
		configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.minDepthBounds = 0.0f;
		configInfo.depthStencilInfo.maxDepthBounds = 1.0f;
		configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
		configInfo.depthStencilInfo.front = {};
		configInfo.depthStencilInfo.back = {};
		//深度测试
		return configInfo;
	}
	//默认管道配置信息函数
	
}