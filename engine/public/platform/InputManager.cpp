#include "InputManager.h"
#include <platform/Window.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <utility>
#include <imgui.h>
#include <cmath>

static InputManager* g_input = nullptr;
static Window* g_window = nullptr;

void InputManager::ResetMouse() { _firstMouse = true; }

void InputManager::OnCursorPos(double xpos, double ypos)
{
	float x = static_cast<float>(xpos);
	float y = static_cast<float>(ypos);
	if (_firstMouse) {
		_lastX = x;
		_lastY = y;
		_firstMouse = false;
		return;
	}
	_dx += x - _lastX;
	_dy += _lastY - y;
	_lastX = x;
	_lastY = y;
}

void InputManager::OnScroll(double yoffset) { _scroll += static_cast<float>(yoffset); }

bool InputManager::ConsumeMouseDelta(float& dx, float& dy)
{
	bool had = (std::abs(_dx) > 1e-6f) || (std::abs(_dy) > 1e-6f);
	dx = _dx; dy = _dy;
	_dx = _dy = 0.0f;
	return had;
}

bool InputManager::ConsumeScroll(float& dy)
{
	bool had = (std::abs(_scroll) > 1e-6f);
	dy = _scroll;
	_scroll = 0.0f;
	return had;
}

void InputManager::SetupInputCallbacks(Window* window)
{
	GLFWwindow* winGL = window->GetWindow();
	g_input = this;
	g_window = window;

	glfwSetCursorPosCallback(winGL, [](GLFWwindow* w, double xpos, double ypos)
		{
			if (g_window->IsCursorCaptured()) g_input->OnCursorPos(xpos, ypos);
		});

	glfwSetScrollCallback(winGL, [](GLFWwindow* w, double xoffset, double yoffset)
		{
			if (g_window->IsCursorCaptured()) g_input->OnScroll(yoffset);
		});

	glfwSetKeyCallback(winGL, [](GLFWwindow* w, int key, int scancode, int action, int mods)
		{
			if (action != GLFW_PRESS) return;

			if (key == GLFW_KEY_F1)
			{
				bool now = !g_window->IsCursorCaptured();
				g_window->SetCursorCaptured(now);
				if (now) g_input->ResetMouse();
			}
			else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				g_window->SetCursorCaptured(false);
		});
}

void InputManager::ClearMouseDelta() { _dx = _dy = _scroll = 0; }