#pragma once

class Window;

class InputManager
{
public:
	void ResetMouse();

	void OnCursorPos(double xpos, double ypos);
	void OnScroll(double yoffset);

	bool ConsumeMouseDelta(float& dx, float& dy);
	bool ConsumeScroll(float& dy);

	void SetupInputCallbacks(Window* window);

	void ClearMouseDelta();

private:
	bool _firstMouse = true;

	float _lastX = 0.0f, _lastY = 0.0f;
	float _dx = 0.0f, _dy = 0.0f;
	float _scroll = 0.0f;
};