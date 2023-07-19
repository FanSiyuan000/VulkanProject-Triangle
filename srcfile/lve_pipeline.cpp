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
		: lveDevice(device)//��Ա��ʼֵ�趨�б��ʼ���豸
	{
		createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
	}//���캯�������ݲ���

	LvePipeline::~LvePipeline() {
		vkDestroyShaderModule(lveDevice.device(), vertShaderModule, nullptr);
		vkDestroyShaderModule(lveDevice.device(), fragShaderModule, nullptr);
		vkDestroyPipeline(lveDevice.device(), graphicsPipeline, nullptr);
	}//����vulkan����

	std::vector<char> LvePipeline::readFile(const std::string& filepath) {
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);//��ȡ��ɫ���ļ�

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file: " + filepath);
		}//����ļ��Ƿ��

		size_t fileSize = static_cast<size_t>(file.tellg());//��ȡ�ļ���С
		std::vector<char> buffer(fileSize);//�ַ�������

		file.seekg(0);
		file.read(buffer.data(), fileSize);//Ѱ���ļ���ͷ����ȡ

		file.close();//�ر��ļ�
		return buffer;
	}

	void LvePipeline::createGraphicsPipeline(const std::string& vertFilepath,
		const std::string& fragFilepath, 
		const PipelineConfigInfo& configInfo) {

		assert(configInfo.pipelineLayout != VK_NULL_HANDLE && "Can not create graphics pipeline: no pipelineLayout provided in connfigInfo.");
		assert(configInfo.renderPass != VK_NULL_HANDLE && "Can not create graphics pipeline: no renderPass provided in connfigInfo.");
		auto vertCode = readFile(vertFilepath);
		auto fragCode = readFile(fragFilepath);//��ȡ�����Լ�Ƭ�δ���
		//����ͼ�ιܵ������Լ�����

		createShaderModule(vertCode, &vertShaderModule);
		createShaderModule(fragCode, &fragShaderModule);//��ʼ����ɫ��ģ��

		VkPipelineShaderStageCreateInfo shaderStages[2];//Ϊ��ɫ�����ø��ഴ����Ϣ
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;//��һ��ʵ����������
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;//��ɫ���׶ζ���λ
		shaderStages[0].module = vertShaderModule;//��������Ϊ����vert��ɫ��ģ��
		shaderStages[0].pName = "main";//������ɫ����ں�������
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;//frag��ɫ��
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};//���Ͷ��㻺�������ݣ���ʼ���뵽ͼ�ιܵ�
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;//����Ϊvk�ṹ���͹ܵ���������״̬
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.vertexBindingDescriptionCount = 0;//�����붥���������������Ϊ0
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &configInfo.viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &configInfo.scissor;
		//���ӿںͼ�����ϳɵ����ӿ�״̬

		VkGraphicsPipelineCreateInfo pipelineInfo{};//��������ͼ�ιܵ�����
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;//��Ա������ָ��ʹ�ö��ٿɱ�̽׶�
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;//�ܵ�������Ϣ���ӵ�������Ϣ
		pipelineInfo.pInputAssemblyState = &configInfo.inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &configInfo.rasterizationInfo;
		pipelineInfo.pMultisampleState = &configInfo.multisampleInfo;
		pipelineInfo.pColorBlendState = &configInfo.colorBlendInfo;
		pipelineInfo.pDepthStencilState = &configInfo.depthStencilInfo;
		pipelineInfo.pDynamicState = nullptr;

		pipelineInfo.layout = configInfo.pipelineLayout;//������Ⱦͨ������ͨ����������
		pipelineInfo.renderPass = configInfo.renderPass;
		pipelineInfo.subpass = configInfo.subpass;

		pipelineInfo.basePipelineIndex = -1;//�����ܵ����
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;//�����ܵ�����

		if (vkCreateGraphicsPipelines(lveDevice.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline"); 
		}

;	}

	void LvePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;//��ɫ��������Ϣ
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());//ָ������ָ�룬��Ҫ������ʹ�����½���ת����Vulkan��Ҫuint32����

		if (vkCreateShaderModule(lveDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module");
		}
	}//����vulkan��ɫ��ģ��
	//vulkan��ʹ�ò������ú�������������һ���ṹ�岢ʹ��ָ������ָ�����һ������

	void LvePipeline::bind(VkCommandBuffer commandBuffer) {
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);//�󶨹ܵ�
	}

	PipelineConfigInfo LvePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
		PipelineConfigInfo configInfo{};

		configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;//s��������Ϊvk�ṹ����
		configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;//���˳�ԱΪvk��Ԫ�����������б�
		configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;//��Ԫ������������ֵΪVK_FALSE

		configInfo.viewport.x = 0.0f;
		configInfo.viewport.y = 0.0f;
		configInfo.viewport.width = static_cast<float>(width);
		configInfo.viewport.height = static_cast<float>(height);//glλ��ת��Ϊ���ͼ��
		configInfo.viewport.minDepth = 0.0f;
		configInfo.viewport.maxDepth = 1.0f;
		//�����ӿ�

		configInfo.scissor.offset = { 0, 0 };
		configInfo.scissor.extent = { width, height };
		//���ü���


		configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;//���ýṹ���ͳ�Ա
		configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;//���ǯλ�������ã�ǿ��glλ��z������0-1��С��0��ǯλ��0������1��ǯλ��1
		configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;//��դ��֮ǰ�������л�Ԫ
		configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;//�����ģ��
		configInfo.rasterizationInfo.lineWidth = 1.0f;
		configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;//ʹ��������������Ψһ��ʶ�����������ε���һ�࣬��Ҫָ����ʱ���˳ʱ��
		configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
		configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;
		configInfo.rasterizationInfo.depthBiasClamp = 0.0f;
		configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;
		//��դ���׶�,��������ֽ�Ϊÿ�����ػ��������ص���Ƭ��

		configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
		configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisampleInfo.minSampleShading = 1.0f;
		configInfo.multisampleInfo.pSampleMask = nullptr;
		configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;
		configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;
		//���ز���

		configInfo.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.colorBlendAttachment.blendEnable = VK_FALSE;//������ɫ��ϣ�����������λ����ɫ��ֵ������ǰ����뻺������������ɫֵ���л��
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
 		//��ɫ��Ͽ���,��֡�������������ɫ

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
		//��Ȳ���
		return configInfo;
	}
	//Ĭ�Ϲܵ�������Ϣ����
	
}