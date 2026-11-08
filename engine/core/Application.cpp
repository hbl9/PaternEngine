#include "Application.h"
#include "../camera/Camera.h"
#include "../game/IGame.h"
#include "../camera/ICameraController.h"
#include "../network/Network.h"
#include "../platform/InputManager.h"
#include "../platform/Window.h"
#include "../scene/Scene.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <utility>

Application::Application(int width, int height, const char* title) :
	_input(std::make_unique<InputManager>()),
	_camera(std::make_unique<Camera>(glm::vec3(0.0f, 1.0f, 4.0f))),
	_scene(std::make_unique<Scene>()),
	_window(std::make_unique<Window>(width, height, title)),
	_network(std::make_unique<Network>()) {
}

Application::~Application()
{
	_game.reset();
	_network.reset();
	_scene.reset();

	if (_window && _window->GetWindow())
	{
		auto* data = static_cast<std::pair<Window*, InputManager*>*>(glfwGetWindowUserPointer(_window->GetWindow()));
		delete data;
	}

	_window.reset();
	glfwTerminate();
}

bool Application::Init(std::unique_ptr<IGame> game, std::unique_ptr<ICameraController> cameraController)
{
	if (!_window) return false;

	if (cameraController) 
	{
		_cameraController = std::move(cameraController);
		_cameraController->SetCamera(_camera.get());
	}

	if (game)
	{
		_game = std::move(game);

		if (_cameraController) _game->SetCameraController(_cameraController.get());
		_game->SetCamera(_camera.get());
		_game->SetNetwork(_network.get());

		_game->OnInit(*_scene);
	}

	_input->SetupInputCallbacks(_window.get());

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	_window->SetGLClearColor(0.2f, 0.2f, 0.3f, 1.0f);
	_lastFrame = static_cast<float>(glfwGetTime());

	return true;
}

void Application::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	_scene->Draw(_camera->GetViewMatrix(), _camera->GetProjectionMatrix(_window->GetAspectRatio()));

	_window->SwapBuffers();
}

void Application::Update(float deltaTime)
{
	if (_cameraController) _cameraController->Update(*_input, *_window, deltaTime);

	if (_game) _game->OnUpdate(deltaTime, *_scene, *_input);

	_window->UpdateFrameBufferSize();
}

void Application::Run()
{
	while (!_window->ShouldClose())
	{
		float currentFrame = static_cast<float>(glfwGetTime());
		float deltaTime = currentFrame - _lastFrame;
		_lastFrame = currentFrame;

		_window->PollEvents();

		Update(deltaTime);
		Render();
	}
}