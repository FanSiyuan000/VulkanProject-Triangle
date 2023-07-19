#pragma once

#include "lve_device.h"

#include <string>
#include <vector>

namespace lve {

	struct PipelineConfigInfo{
		VkViewport viewport;//�ӿ�
		VkRect2D scissor;//����
		//��ϳɵ����ӿ�״̬
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;//��դ��
		VkPipelineMultisampleStateCreateInfo multisampleInfo;//���ز���
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;//��ɫ���
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;//��Ȳ���
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};//���ùܵ���ͬ�׶ι�����ʽ��ֵ

	class  LvePipeline
	{
	public:
		 LvePipeline(
			 LveDevice& device, 
			 const std::string& vertFilepath, 
			 const std::string& fragFilepath, 
			 const PipelineConfigInfo& configInfo);//�Զ����ļ�·�����г����ַ�������

		 ~LvePipeline();

		 LvePipeline(const LvePipeline&) = delete;
		 void operator = (const LvePipeline) = delete;//ɾ�����ƹ��캯�������������ظ�ָ��Vulkan�����ָ��

		 void bind(VkCommandBuffer commandBuffer);

		 static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);//����Ĭ�Ϲܵ�����

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(
			const std::string& vertFilepath, 
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo);//�����ܵ�����

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);//����ģ�鲢��ʼ������

		LveDevice& lveDevice;//�����豸����
		VkPipeline graphicsPipeline;//Vulkan�ܵ�����
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;//��ɫ��ģ��
	};

}