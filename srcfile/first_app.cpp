#include "first_app.h"

#include <stdexcept>
#include <array>

namespace lve {

	FirstApp::FirstApp() {
		loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();//调用
	}

	FirstApp::~FirstApp() {
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}

	void FirstApp::run() {

		while (!lveWindow.shouldClose()) {//为窗口调用一个虚构函数，是否关闭
			glfwPollEvents();//检查与处理窗口级别的事件
			drawFrame();
		}

		vkDeviceWaitIdle(lveDevice.device());//CPU将封锁直到GPU操作完成
	}

	void FirstApp::loadModels() {
		std::vector<LveModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{ {0.5f, 0.5f}, {0.0f, 1.0f, 0.0f} },
			{  {-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}  }};
		lveModel = std::make_unique<LveModel>(lveDevice, vertices);
	}

	void FirstApp::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;//用于传送数据，除了顶点数据到顶点和片段着色器之外，可以包括纹理以及统一缓冲区对象
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;//向着色器发送少量数据

		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout.");
		}
	}

	void FirstApp::createPipeline() {
		auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(lveSwapChain.width(), lveSwapChain.height());
		//使用swapchain（交换链）高度宽度，因为不一定符合屏幕像素（窗口匹配）
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
		
		commandBuffers.resize(lveSwapChain.imageCount());//imageCount的值可能是2或3，取决于设备是否支持双缓冲或是三重缓冲

		VkCommandBufferAllocateInfo allocInfo{};//分配命令缓冲区
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;//primary命令缓冲区可以提交到队列执行，但不能被其他命令缓冲区调用，secondary命令缓冲区不能提交，但能被其它命令缓冲区调用
		allocInfo.commandPool = lveDevice.getCommandPool();//从命令池中分配和释放命令缓冲区，减少资源创建成本
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
		
		if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers");
		}

		for (int i = 0; i < commandBuffers.size(); i++) {//将命令记录绘制到每个缓冲区
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			
			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer");
			}
		
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = lveSwapChain.getRenderPass();//设置渲染通道成员
			renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);//渲染通道写入哪个帧缓冲区

			renderPassInfo.renderArea.offset = { 0, 0 };//设置渲染区域（着色器加载和储存的区域）
			renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};//清除帧缓冲区附件的初始值
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 0.1f };//RGBA值
			clearValues[1].depthStencil = { 1.0f, 0 };//深度值 1-0
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);//记录到命令缓冲区以开始渲染过程
		
			lvePipeline->bind(commandBuffers[i]);
			lveModel->bind(commandBuffers[i]);
			lveModel->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);//完成记录
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer");
			}
		}
	};
	void FirstApp::drawFrame() {
		uint32_t imageIndex;
		auto result = lveSwapChain.acquireNextImage(&imageIndex);//获取应该渲染到下一帧的索引，自动处理CPU与GPU围绕双缓冲或三重缓冲返回的结果值确定过程是否成功
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image");
		}

		result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		//将提供的命令缓冲区提交到设备图形队列，同时处理CPU,GPU同步
		//然后执行命令缓冲区，swapchain将根据所选的当前模式在适当的时间将关联的颜色附件视图呈现给显示器
		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swwap chain image");
		}
	};

}