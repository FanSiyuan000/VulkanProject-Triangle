#pragma once

#include "lve_window.h"
#include "lve_pipeline.h"
#include "lve_device.h"
#include "lve_swap_chain.h"
#include "lve_model.h"

#include <memory>
#include <vector>

namespace lve {
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;//���崰�ڳ���

		FirstApp();
		~FirstApp();//firstapp���ڹ���ܵ����ֺ�command buffer��vulkan����

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator = (const FirstApp&) = delete;
		//ɾ�����ƹ��캯���븴�������
		void run();//���к���

	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		LveWindow lveWindow{WIDTH, HEIGHT, "Vulkan"};//��������һ�����ڣ�����ʱ��window�Զ��ƻ�
		LveDevice lveDevice{ lveWindow };
		LveSwapChain lveSwapChain{ lveDevice, lveWindow.getExtent() };

		std::unique_ptr<LvePipeline> lvePipeline;//����ָ��
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<LveModel> lveModel;
		//ʹ�ܵ������ΪΨһ��ָ��
		//LvePipeline lvePipeline{lveDevice, 
		//	"shaders/simple_shader.vert.spv", 
		//	"shaders/simple_shader.frag.spv", 
		//	LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};//ʵ�����ܵ�,ע���ȡ.spv�ļ�
	};
}