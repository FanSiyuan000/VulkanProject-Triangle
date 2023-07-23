#pragma once

#include "lve_device.h"

// std
#include <string>
#include <vector>

namespace lve {

    struct PipelineConfigInfo {
        PipelineConfigInfo() = default;
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;//��դ��
        VkPipelineMultisampleStateCreateInfo multisampleInfo;//���ز���
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;//��ɫ���
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;//��Ȳ���
        std::vector<VkDynamicState> dynamicStateEnables;
        VkPipelineDynamicStateCreateInfo dynamicStateInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };//���ùܵ���ͬ�׶ι�����ʽ��ֵ

    class LvePipeline {
    public:
        LvePipeline(
            LveDevice& device,
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);//�Զ����ļ�·�����г����ַ�������

        ~LvePipeline();
        //ɾ�����ƹ��캯�������������ظ�ָ��Vulkan�����ָ��
        LvePipeline(const LvePipeline&) = delete;
        LvePipeline& operator=(const LvePipeline&) = delete;

        void bind(VkCommandBuffer commandBuffer);

        static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);//����Ĭ�Ϲܵ�����

    private:
        static std::vector<char> readFile(const std::string& filepath);

        void createGraphicsPipeline(
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);

        void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

        LveDevice& lveDevice;
        VkPipeline graphicsPipeline;
        VkShaderModule vertShaderModule;
        VkShaderModule fragShaderModule;
    };
}