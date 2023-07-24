#pragma once

#include "lve_device.h"
#include "lve_swap_chain.h"
#include "lve_window.h"

#include <cassert>
#include <memory>
#include <vector>

namespace lve {
	class LveRenderer {
	public:

		LveRenderer(LveWindow &window, LveDevice &device);
		~LveRenderer();//LveRenderer���ڹ���ܵ����ֺ�command buffer��vulkan����

		LveRenderer(const LveRenderer&) = delete;
		LveRenderer& operator=(const LveRenderer&) = delete;
		//ɾ�����ƹ��캯���븴�������

		VkRenderPass getSwapChainRenderPass() const { return lveSwapChain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:

		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		LveWindow& lveWindow;//��������һ�����ڣ�����ʱ��window�Զ��ƻ�
		LveDevice& lveDevice;
		std::unique_ptr<LveSwapChain> lveSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex;
		bool isFrameStarted;
		//ʹ�ܵ������ΪΨһ��ָ��
		//LvePipeline lvePipeline{lveDevice, 
		//	"shaders/simple_shader.vert.spv", 
		//	"shaders/simple_shader.frag.spv", 
		//	LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};//ʵ�����ܵ�,ע���ȡ.spv�ļ�
	};
}