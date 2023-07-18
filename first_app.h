#pragma once

#include "lve_window.h"

namespace lve {
	class FirstApp
	{
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;//定义窗口常量

		void run();//运行函数

	private:
		LveWindow lveWindow{WIDTH, HEIGHT, "Vulkan"};//创建并打开一个窗口，销毁时，window自动破坏
	};
}