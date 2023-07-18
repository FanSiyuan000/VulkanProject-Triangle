#include "first_app.h"

namespace lve {
	void FirstApp::run() {

		while (!lveWindow.shouldClose()) {//为窗口调用一个虚构函数，是否关闭
			glfwPollEvents();//检查与处理窗口级别的事件
		}
	}
}