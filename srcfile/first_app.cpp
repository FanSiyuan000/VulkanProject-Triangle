#include "first_app.h"

#include <stdexcept>
#include <array>

namespace lve {

	FirstApp::FirstApp() {
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();//����
	}

	FirstApp::~FirstApp() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}

	void FirstApp::run() {

		while (!lveWindow.shouldClose()) {//Ϊ���ڵ���һ���鹹�������Ƿ�ر�
			glfwPollEvents();//����봦�����ڼ�����¼�
			drawFrame();
		}

		vkDeviceWaitIdle(lveDevice.device());//CPU������ֱ��GPU�������
	}

	void FirstApp::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;//���ڴ������ݣ����˶������ݵ������Ƭ����ɫ��֮�⣬���԰��������Լ�ͳһ����������
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;//����ɫ��������������

		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout.");
		}
	}

	void FirstApp::createPipeline() {
		auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(lveSwapChain.width(), lveSwapChain.height());
		//ʹ��swapchain�����������߶ȿ��ȣ���Ϊ��һ��������Ļ���أ�����ƥ�䣩
		pipelineConfig.renderPass = lveSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(
			lveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig
		);
	}

	void FirstApp::createCommandBuffers() {
		
		commandBuffers.resize(lveSwapChain.imageCount());//imageCount��ֵ������2��3��ȡ�����豸�Ƿ�֧��˫����������ػ���

		VkCommandBufferAllocateInfo allocInfo{};//�����������
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//primary������������ύ������ִ�У������ܱ���������������ã�secondary������������ύ�����ܱ����������������
		allocInfo.commandPool = lveDevice.getCommandPool();//��������з�����ͷ����������������Դ�����ɱ�
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		
		if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}

		for (int i = 0; i < commandBuffers.size(); i++) {//�������¼���Ƶ�ÿ��������
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			
			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer");
			}
		
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = lveSwapChain.getRenderPass();//������Ⱦͨ����Ա
			renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);//��Ⱦͨ��д���ĸ�֡������

			renderPassInfo.renderArea.offset = { 0, 0 };//������Ⱦ������ɫ�����غʹ��������
			renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};//���֡�����������ĳ�ʼֵ
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };//RGBAֵ
			clearValues[1].depthStencil = { 1.0f, 0 };//���ֵ 1-0
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);//��¼����������Կ�ʼ��Ⱦ����
		
			lvePipeline->bind(commandBuffers[i]);
			vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(commandBuffers[i]);//��ɼ�¼
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer");
			}
		}
	};
	void FirstApp::drawFrame() {
		uint32_t imageIndex;
		auto result = lveSwapChain.acquireNextImage(&imageIndex);//��ȡӦ����Ⱦ����һ֡���������Զ�����CPU��GPUΧ��˫��������ػ��巵�صĽ��ֵȷ�������Ƿ�ɹ�
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		//���ṩ����������ύ���豸ͼ�ζ��У�ͬʱ����CPU,GPUͬ��
		//Ȼ��ִ�����������swapchain��������ѡ�ĵ�ǰģʽ���ʵ���ʱ�佫��������ɫ������ͼ���ָ���ʾ��
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swwap chain image");
		}
	};

}