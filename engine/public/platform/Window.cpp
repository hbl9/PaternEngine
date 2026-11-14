#include "Window.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <glad.h>

Window::Window(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share, FrameRateMode frameRateMode)
{
	if (!glfwInit()) throw std::runtime_error("[Window] GLFW initialization failed");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, title, monitor, share);
	if (!window) throw std::runtime_error("[Window] Window creation failed");

	_window = window;

	glfwMakeContextCurrent(_window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw std::runtime_error("[Window] Failed to load OpenGL function pointers");

	glViewport(0, 0, width, height);

	if (frameRateMode == FrameRateMode::VSync)
		glfwSwapInterval(1);  // VSync
	else
		glfwSwapInterval(0);  // Unlimited

	glfwSetFramebufferSizeCallback(_window, [](GLFWwindow*, int w, int h) { glViewport(0, 0, w, h); });
	UpdateFrameBufferSize();

}

Window::~Window() { Destroy(); }

bool Window::IsCursorCaptured() const { return _cursorCaptured; }

void Window::MakeCurrent() { glfwMakeContextCurrent(_window); }

void Window::SetVSync(FrameRateMode frameRateMode) { glfwSwapInterval(frameRateMode == FrameRateMode::VSync ? 1 : 0); }
void Window::SetGLClearColor(float r, float g, float b, float a) { glClearColor(r, g, b, a); }
void Window::SetCursorCaptured(bool captured)
{
	_cursorCaptured = captured;
	glfwSetInputMode(_window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window::UpdateFrameBufferSize() { glfwGetFramebufferSize(_window, &_framebufferWidth, &_framebufferHeight); }

GLFWwindow* Window::GetWindow() const { return _window; }
glm::ivec2 Window::GetFramebufferSize() const { return glm::ivec2(_framebufferWidth, _framebufferHeight); }
float Window::GetAspectRatio() const { return _framebufferHeight == 0 ? 1 : _framebufferWidth / static_cast<float>(_framebufferHeight); }

void Window::PollEvents() { glfwPollEvents(); }

void Window::SwapBuffers() { glfwSwapBuffers(_window); }

void Window::Destroy()
{
	if (_window)
	{
		glfwDestroyWindow(_window);
		_window = nullptr;
	}
}

bool Window::ShouldClose() const { return glfwWindowShouldClose(_window); }