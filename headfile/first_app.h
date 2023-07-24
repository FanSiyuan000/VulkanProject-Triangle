#pragma once

#include "lve_device.h"
#include "lve_game_object.h"
#include "lve_window.h"
#include "lve_renderer.h"

#include <memory>
#include <vector>

namespace lve {
	class FirstApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;//定义窗口常量

		FirstApp();
		~FirstApp();//firstapp正在管理管道布局和command buffer的vulkan对象//firstapp正在管理管道布局和command buffer的vulkan对象

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;
		//删除复制构造函数与复制运算符
		void run();//运行函数

	private:
		void loadGameObjects();

		LveWindow lveWindow{ WIDTH, HEIGHT, "Vulkan Tutorial" };//创建并打开一个窗口，销毁时，window自动破坏
		LveDevice lveDevice{ lveWindow };
		LveRenderer lveRenderer{lveWindow, lveDevice};

		std::vector<LveGameObject> gameObjects;
		//使管道对象成为唯一的指针
		//LvePipeline lvePipeline{lveDevice, 
		//	"shaders/simple_shader.vert.spv", 
		//	"shaders/simple_shader.frag.spv", 
		//	LvePipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};//实例化管道,注意读取.spv文件
	};
}