#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <GLFW/glfw3.h>
#include <AL/al.h>
#include <AL/alc.h>

static GLFWwindow *glfw_window = 0;
static ALCdevice *alc_device = 0;
static ALCcontext *alc_context = 0;

static int prepare() {
	glfwSetErrorCallback([](int error, const char *desc) {
		std::cerr << "GLFW error #" << error << ": " << desc << std::endl;
	});
	if (glfwInit() == GLFW_FALSE) return 1;
	glfw_window = glfwCreateWindow(800, 600, "GLFW", 0, 0);
	if (!glfw_window) return 2;
	glfwMakeContextCurrent(glfw_window);
	alc_device = alcOpenDevice(0);
	if (!alc_device) {
		std::cout << "Failed to open default audio device." << std::endl;
		return false;
	}
	alc_context = alcCreateContext(alc_device, 0);
	if (!alcMakeContextCurrent(alc_context)) {
		std::cout << "Failed to activate audio context." << std::endl;
		return false;
	}
	return 0;
}

static void cleanup() {
	if (alc_context) alcDestroyContext(alc_context);
	alc_context = 0;
	if (alc_device) alcCloseDevice(alc_device);
	alc_device = 0;
	if (glfw_window) glfwDestroyWindow(glfw_window);
	glfw_window = 0;
	glfwTerminate();
}

static void loop() {
	glClearColor(.2, .3, .4, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(glfw_window);
	glfwPollEvents();
}

static void run() {
	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(loop, 0, true);
	#else
	while (!glfwWindowShouldClose(glfw_window)) loop();
	#endif
}

int main() {
	std::atexit(cleanup);
	if (int prep_res = prepare(); prep_res != 0) return prep_res;
	run();
}