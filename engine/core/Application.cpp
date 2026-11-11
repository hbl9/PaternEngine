#include "Application.h"
#include "../camera/Camera.h"
#include "../game/IGame.h"
#include "../camera/ICameraController.h"
#include "../network/Network.h"
#include "../platform/InputManager.h"
#include "../platform/Window.h"
#include "../scene/Scene.h"
#include "../scene/SceneObject.h"
#include "../scene/Picking.h"
#include "application/EditorLayer.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <utility>
#include <imgui.h>
#include <limits>

Application::Application(int width, int height, const char* title) :
	_input(std::make_unique<InputManager>()),
	_camera(std::make_unique<Camera>(glm::vec3(0.0f, 1.0f, 4.0f))),
	_scene(std::make_unique<Scene>()),
	_window(std::make_unique<Window>(width, height, title)),
	_network(std::make_unique<Network>()) {
}

Application::~Application()
{
	_editor.reset();
	_game.reset();
	_network.reset();
	_scene.reset();
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

	_editor = std::make_unique<EditorLayer>(*_window, *_scene, *_input);

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

	_scene->Render(_camera->GetViewMatrix(), _camera->GetProjectionMatrix(_window->GetAspectRatio()));
}

void Application::Update(float deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();

	if (!_window->IsCursorCaptured() && ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
		!io.WantCaptureMouse && !io.WantTextInput)
	{
		ImVec2 mp = ImGui::GetMousePos();
		std::uint64_t picked = PickObjectAt(mp.x, mp.y);

		if (_editor) _editor->SetSelected(picked);
	}

	bool uiCapture = io.WantCaptureMouse || io.WantTextInput;
	if (_cameraController && !uiCapture && _window->IsCursorCaptured())
		_cameraController->Update(*_input, *_window, deltaTime);
	else
		_input->ClearMouseDelta();

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

		if (_editor) _editor->BeginFrame();

		Update(deltaTime);

		Render();

		if (_editor)
		{
			_editor->Draw();
			_editor->EndFrame();
		}

		_window->SwapBuffers();
	}
}

std::uint64_t Application::PickObjectAt(float mouseX, float mouseY)
{
	int winW, winH;
	glfwGetWindowSize(_window->GetWindow(), &winW, &winH);
	if (winW == 0 || winH == 0) return SceneHandle::INVALID;

	float ndcX = (2.0f * mouseX / winW) - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseY / winH);

	glm::vec3 rayOrigin(0.0f), rayDirection(0.0f);
	_camera->GetRayFromScreen(ndcX, ndcY, static_cast<float>(winW) / winH, rayOrigin, rayDirection);

	std::uint64_t best = SceneHandle::INVALID;
	float bestT = std::numeric_limits<float>::max();

	for (std::uint64_t id : _scene->GetRootsId())
	{
		std::vector<std::uint64_t> stack{ id };
		while (!stack.empty())
		{
			std::uint64_t cur = stack.back();
			stack.pop_back();
			SceneObject* obj = _scene->GetObjById(cur);

			for (std::uint64_t children : _scene->GetChildren(cur))
				stack.push_back(children);

			if (!obj || obj->IsNothing()) continue;

			glm::mat4 invModel = glm::inverse(obj->GetModelMatrix());
			glm::vec3 localOrigin = glm::vec3(invModel * glm::vec4(rayOrigin, 1.0f));
			glm::vec3 localDirection = glm::vec3(invModel * glm::vec4(rayDirection, 0.0f));
			glm::vec3 boundingMin = obj->GetLocalAabbMin();
			glm::vec3 boundingMax = obj->GetLocalAabbMax();

			if (Picking::PointInsideAabb(localOrigin, boundingMin, boundingMax)) continue;

			float t = Picking::RayAabbLocal(localOrigin, localDirection, obj->GetLocalAabbMin(), obj->GetLocalAabbMax());
			if (t < 0.0f) continue;

			glm::vec3 localHit = localOrigin + localDirection * t;
			glm::vec3 worldHit = glm::vec3(obj->GetModelMatrix() * glm::vec4(localHit, 1.0f));
			float worldT = glm::length(worldHit - rayOrigin);
			if (worldT < bestT)
			{
				bestT = worldT;
				best = cur;
			}
		}
	}
	return best;
}