#include "lve_window.h"

#include <stdexcept>

namespace lve {
	LveWindow::LveWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();//调用initWindow
	}//构造函数使用成员初始值设定项列表来初始化成员变量

	LveWindow::~LveWindow() {
		glfwDestroyWindow(window);//命令传入window指针
		glfwTerminate();
	}//析构函数销毁初始化所需资源

	void LveWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);//不创建OpenGL上下文
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);//使用提示在创建后禁用窗口大小调整

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);//创建窗口命令

	}//实现initWindow，需要调用glfwInit初始化GLFW库

	void LveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface");
		}
	}
}