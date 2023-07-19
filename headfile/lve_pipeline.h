#pragma once

#include "lve_device.h"

#include <string>
#include <vector>

namespace lve {

	struct PipelineConfigInfo{
		VkViewport viewport;//视口
		VkRect2D scissor;//剪刀
		//组合成单个视口状态
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;//光栅化
		VkPipelineMultisampleStateCreateInfo multisampleInfo;//多重采样
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;//颜色混合
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;//深度测试
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};//配置管道不同阶段工作方式的值

	class  LvePipeline
	{
	public:
		 LvePipeline(
			 LveDevice& device, 
			 const std::string& vertFilepath, 
			 const std::string& fragFilepath, 
			 const PipelineConfigInfo& configInfo);//对顶点文件路径进行常量字符串引用

		 ~LvePipeline();

		 LvePipeline(const LvePipeline&) = delete;
		 void operator = (const LvePipeline) = delete;//删除复制构造函数，避免意外重复指向Vulkan对象的指针

		 void bind(VkCommandBuffer commandBuffer);

		 static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);//创建默认管道配置

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(
			const std::string& vertFilepath, 
			const std::string& fragFilepath, 
			const PipelineConfigInfo& configInfo);//创建管道函数

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);//创建模块并初始化变量

		LveDevice& lveDevice;//储存设备引用
		VkPipeline graphicsPipeline;//Vulkan管道对象
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;//着色器模块
	};

}