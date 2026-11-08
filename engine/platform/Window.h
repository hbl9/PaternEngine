#pragma once

#include <glm/vec2.hpp>

struct GLFWwindow;
struct GLFWmonitor;

enum class FrameRateMode { VSync, Unlimited };

class Window
{
public:
	Window() = default;
	Window(int width = 600, int height = 600, const char* title = "Window",
		GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr, FrameRateMode frameRateMode = FrameRateMode::VSync);
	~Window();

	bool IsCursorCaptured() const;
	bool ShouldClose() const;

	void SetCursorCaptured(bool captured);
	void SetVSync(FrameRateMode frameRateMode);
	void SetGLClearColor(float r, float g, float b, float a);
	float GetAspectRatio() const;

	void MakeCurrent();

	GLFWwindow* GetWindow() const;
	glm::ivec2 GetSizeWindow() const;

	void UpdateFrameBufferSize();
	void SwapBuffers();
	void PollEvents();

	void Destroy();

private:
	GLFWwindow* _window = nullptr;
	int _width = 0, _height = 0;

	bool _cursorCaptured = false;
};