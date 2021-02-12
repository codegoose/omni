#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#else
#include <glad/glad.h>
#endif

#include <GLFW/glfw3.h>
#include <AL/al.h>
#include <AL/alc.h>

#include <imgui.h>
#include <imgui_freetype.h>
#include <spdlog/spdlog.h>
#include <fire.hpp>

static GLFWwindow *glfw_window = 0;
static ALCdevice *alc_device = 0;
static ALCcontext *alc_context = 0;

static bool arg_gl_enable_dbg = false;

namespace app {
	void on_loop();
}

static void loop() {
	app::on_loop();
	glClearColor(.7, .5, .3, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(glfw_window);
	glfwPollEvents();
}

static int prepare() {
	glfwSetErrorCallback([](int error, const char *desc) { std::cerr << "GLFW error #" << error << ": " << desc << std::endl; });
	if (glfwInit() == GLFW_FALSE) return 1;
	glfw_window = glfwCreateWindow(800, 600, "GLFW", 0, 0);
	if (!glfw_window) return 2;
	glfwMakeContextCurrent(glfw_window);
	#ifndef __EMSCRIPTEN__
	if (gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0) {
		spdlog::error("Failed to GL extensions.");
		return 3;
	}
	if (arg_gl_enable_dbg) {
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback([](GLenum src, GLenum type, GLuint id, GLenum sev, GLsizei len, const GLchar *msg, const void *user_ptr) {
			spdlog::error("GL error #{}: {}", id, msg);
		}, 0);
		spdlog::info("GL debugging enabled.");
	}
	#if defined WIN32 || defined _WIN32
	// On Windows, event polling will not return while the window is being resized.
	// Resize messages still occur, so I call render on each one of those.
	// If the user were to grab the window edge, activating the resize operation
	// but not actually move the mouse; that would still essentially freeze the application.
	// Other OS's don't behave this way, hence the ifdef's.
	glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow *window, int w, int h) { loop(); });
	#endif // Not Windows
	#endif // Not Emscripten
	alc_device = alcOpenDevice(0);
	if (!alc_device) {
		spdlog::warn("Unable to open default audio device.");
		return 4;
	}
	alc_context = alcCreateContext(alc_device, 0);
	if (!alcMakeContextCurrent(alc_context)) {
		spdlog::warn("Unable to activate audio context.");
		return 5;
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

static void run() {
	#ifdef __EMSCRIPTEN__
	// The browser will manage how often the loop is called.
	emscripten_set_main_loop(loop, 0, true);
	#else
	while (!glfwWindowShouldClose(glfw_window)) loop();
	#endif
}

int fired_main(bool dbg_gl = fire::arg({ "-g", "--gldbg", "Enable GL error debug messages." })) {
	if (dbg_gl) arg_gl_enable_dbg = true;
	std::atexit(cleanup);
	if (int prep_res = prepare(); prep_res != 0) return prep_res;
	run();
	return 0;
}

FIRE(fired_main, "This is an epic program.")